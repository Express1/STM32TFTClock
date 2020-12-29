/*
   tft lcd Clock STM32f1 Arduino

   Copyright 2020 Radu Constantinescu
   https://github.com/Express1/STM32Clock

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.
*/

//Generic STM32F103C series, STM32F103CB (20k RAM, 128k Flash) 48Mhz (slow - with USB) or 72 Mhz. Optimize -O2
// Additional Boards manger URL:http://dan.drown.org/stm32duino/package_STM32duino_index.json

//NEO gps library
#include <NMEAGPS.h>
#include <GPSport.h>
//https://github.com/SlashDevin/NeoGPS
NMEAGPS gps; // This parses the GPS characters
gps_fix fix; // This holds on to the latest values
//Serial 3 Rx PB11, Tx PB10 connected to GPS NMEA 9600

/*
  //NMEAGPS_cfg.h

  //for NEO 6M, default
  //#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_RMC

  //for NEO 6M,8M
  //Enable GLL
  //#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL
*/

uint32_t hmin, hmax;

#include "clock.h" // this file is in the scketch folder
//#include "clock.ino"
//#include "copyprogmem.ino"
#include "thickline.h"
//#include "thickline.ino"

//DS18B20 temp sensor
#include <OneWire.h>
#include <DS18B20.h>
#define ONE_WIRE_BUS PB12
OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensor(&oneWire);

// built in RTC library for STM32F1
#include <RTClock.h>
RTClock rtclock(RTCSEL_LSE);

// >> map "Serial" to Serial1 <<<
#define Serial Serial1

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341_STM.h"
#include <itoa.h>

// TFT display 2.8' ili9341
#define TFT_DC PB14
#define TFT_CS PA4
#define TFT_RST PB13

#define TFT_MOSI PB5
#define TFT_MISO PB4
#define TFT_CLK PB3

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST);

//internal rtc
tm_t mtt;
time_t tt;

#define plusbutton 1
#define minusbutton 2

//seconds interrupt flag RTC
volatile unsigned int secflag;
volatile unsigned int almflag;

//char received = 0;
//long mymillis = 0;
// gps variables

unsigned long fix_age, timeg, dateg;
//speedg, courseg;
//unsigned long chars;
//unsigned short sentences, failed_checksum;

#include <TaskScheduler.h>

#define Analog_Button PA2 //analog pin where the buttons are connected
#define PINSPK PB15       // speaker to ground
#define PINTFTLED PA8     //PWM for lcd backlight
#define debug_s

//buttons
#define HH 300  //hysteresis
#define RR 4096 // resolution

char buffer[32]; //buffer
char ast[10];    //a string

uint8_t seconds;                // seconds from timer chip
uint8_t minutes, hours, hours1; // last min value
uint8_t setdst = 0; // used to set dst only once
uint8_t alarmon; //used durring alarm

uint8_t mode;  // 0 = clock, 1 = BCD, 2 = setup
uint8_t menui; //menuitem

bool backup = false; //save to rom

// menu callback routines

// set time
void settime1(void)
{
  //read time and keep seconds
  rtclock.breakTime(rtclock.TimeZone(rtclock.getTime(), menuval[17]), mtt);
  mtt.minute = menuval[1];
  mtt.hour = menuval[0];
  mtt.day = menuval[3];
  mtt.month = menuval[2];
  mtt.weekday = menuval[4];
  mtt.year = menuval[5] + 30;
  tt = rtclock.makeTime(mtt);
  tt -= menuval[17] * 3600; // RTC is UTC
  rtclock.setTime(tt);
  mode = 30; //go back to setup
  Serial.println(F("Set 1"));
}

void settimemin(void)
{
  mtt.second = 0; //reset seconds
  mtt.minute = menuval[1];
  mtt.hour = menuval[0];
  mtt.day = menuval[3];
  mtt.month = menuval[2];
  mtt.weekday = menuval[4];
  mtt.year = menuval[5] + 30;
  tt = rtclock.makeTime(mtt);
  tt -= menuval[17] * 3600; // RTC is UTC
  rtclock.setTime(tt);
  mode = 30; //go back to setup
  Serial.println(F("Sets 2"));
}

void exitmenu2(void)
{
  mode = 30; //go back to setup
  // write settings to battery backed ram
  backup = true;
  //writeArrayToRom(); //ignore first 6
  Serial.println(F("Ex 2"));
}

void setexit3(void)
{
  //tft.fillRect(0, 0, 240, 140, ILI9341_BLACK);
  //clearrow(260, 2);
  //clearrow(280, 2);
  seconds--;
  minutes--; // force update sec,min, hour
  hours--;
  mode = menuval[7]; // exit menu, go back to clock

  // write an array to the rom.
  backup = true;
  //writeArrayToRom(); //ignore first 6

  tft.fillScreen(ILI9341_BLACK);
  if (menuval[7] == 3)
    drawclock(); //analog clock
  //myclock();

  Serial.println(F("Ex 3"));
}

//lcd day backlight
void setlcd(void)
{
  pwmWrite(PINTFTLED, menuval[15] * 256 * 16);
  mode = 30; //go back to setup
}
//lcd night backlight
void setlcdn(void)
{
  pwmWrite(PINTFTLED, menuval[16] * 256 * 16);
  mode = 30; //go back to setup
}

// used to change the LCD backlight, military time
#define STARTDAY 7
#define STARTNIGHT 22

// Callback methods prototypes
void t1Callback();
void t2Callback();
void t3Callback();
void t4Callback();

void KUCallback(); // key up
void KMCallback(); // key menu
void KDCallback(); // key down
//void UpdateScreen(); //redraw screen, read GPS clock

//Tasks
Scheduler runner;

Task t1(100, TASK_FOREVER, &t1Callback);  // call the check button function ever 50 ms
Task t2(1000, TASK_FOREVER, &t2Callback); //each second
Task t3(1000, 31, &t3Callback);       //alarm task 30 sec+1
Task t4(100, 6, &t4Callback);         //alarm task 2


bool key_press = false;  // no key pressed on previous iteration
bool key_press1 = false; //wait for the menu to finish befoore sending anither key
long key_press2;  // menu timeout
int key_value; // ADC read



// called when the menu button is pressed
void onmenubutton()
{
  if (alarmon == 1)
  { //alarm stop
    stopalarm();
  }
  else
  {
    MenuRecord thisItem;
    //Serial.println(F("Button mIN"));
    switch (mode)
    {
      case 0: //digi
      case 1: //bcd
      case 2: //analog
      case 3: //gps
        {
          mode = 30; //setup mode
          menui = 0;
          clearrow(260, 2);
          strcpy(buffer, "Setup");
          tftcprint(ILI9341_RED, 260, 2);
          clearrow(280, 2);
          clearrow(300, 2);
          PROGMEM_readAnything(&menu[menui], thisItem);
          strcpy(buffer, thisItem.item);
          tftcprint(ILI9341_RED, 280, 2);
        }
        break;
      case 30:
        { //show value to change
          if (menui == MENUCOUNT - 1)
          { //exit menu selection
            setexit3();
          }
          else
          {
            PROGMEM_readAnything(&menu[menui], thisItem);
            mode = 40; // the menu is selected to change values
            // show value on screen
            if (thisItem.values == NULL)
            {
              itoa(menuval[menui], buffer, 10);
            }
            else
            {
              //read day of week from menu record *
              memcpy_P(&buffer, &thisItem.values[menuval[menui] - thisItem.start], CHARCOUNT);
            }
            tftcprint(ILI9341_RED, 300, 2);
          }
        }
        break;
      case 40:
        { //write value and go back to menu
          PROGMEM_readAnything(&menu[menui], thisItem);
          // now write the values and go back to mode 3
          thisItem.set();
          //mode = 3;
          //fix screen, delete line @ 300
          clearrow(300, 2);
          //tftcprint(ILI9341_RED, 300, 2);
        }
        break;
    }
  }
  //key_press1 = false;
  //Serial.println(F("Button mO"));
}

// called when + or - are pressed
void onminusbutton(int pin)
{
  MenuRecord thisItem;
  PROGMEM_readAnything(&menu[menui], thisItem);
  //Serial.println(F("Button +-I"));
  switch (mode)
  {
    case 30:
      {
        if (pin == minusbutton)
        {
          menui--;
          if (menui == 255)
            menui = MENUCOUNT - 1;
        }
        if (pin == plusbutton)
        {
          menui++;
          if (menui == MENUCOUNT)
            menui = 0;
        }

        clearrow(280, 2);
        PROGMEM_readAnything(&menu[menui], thisItem);
        strcpy(buffer, thisItem.item);
        tftcprint(ILI9341_RED, 280, 2);
      }
      break;
    case 40:
      {
        if (pin == minusbutton)
        {
          if (menuval[menui] == thisItem.start)
          {
            menuval[menui] = thisItem.count;
          }
          else
          {
            menuval[menui]--;
          }
        }

        if (pin == plusbutton)
        {
          if (menuval[menui] == thisItem.count)
          {
            menuval[menui] = thisItem.start;
          }
          else
          {
            menuval[menui]++;
          }
        }

        clearrow(300, 2);
        if (thisItem.values == NULL)
        {
          itoa(menuval[menui], buffer, 10);
        }
        else
        {
          //read day of week from menu record *
          memcpy_P(&buffer, &thisItem.values[menuval[menui] - thisItem.start], CHARCOUNT);
        }
        tftcprint(ILI9341_RED, 300, 2);
      }
      break;
  }
  //key_press1 = false;
  //Serial.println(F("Button +-O"));
}

//-----------------------------------------------------------------------------
// This function is called in the attachSecondsInterrupt
//-----------------------------------------------------------------------------
void SecondCount()
{
  secflag++;
}
//-----------------------------------------------------------------------------
// This function is called in the attachAlarmInterrupt
//-----------------------------------------------------------------------------
void AlarmInt()
{
  almflag++;
}

void setup()
{
  //Serial.begin(115200);

  //Serial3.begin(9600);

  //
  DEBUG_PORT.begin(115200);
  DEBUG_PORT.print(F("GPS Clock\n"));
  gpsPort.begin(9600);
  //hmin = 0;
  //hmax = 0;

  tft.begin();

  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  tft.setTextSize(2);
  tft.setTextWrap(true);

  tft.println(F("STM32 GPS Clock"));
  tft.println(F("2020 Radu C"));


  Serial.println();

  //speaker
  pinMode(PINSPK, OUTPUT);
  pinMode(PC13, INPUT); //backup battery PC13  is in the power backup domain

  //LCD Backlight
  pinMode(PINTFTLED, PWM);
  pwmWrite(PINTFTLED, 15 * 256 * 16);

  //Analog input pin for keys
  pinMode(Analog_Button, INPUT);

  //DS18B20 tempp sensor
  sensor.begin();
  sensor.requestTemperatures();

  //////////////////////////////////////////////////////////////
  //read values form EEPROM or battery backed RAM
  //readArray(menuval + 6, sizeof menuval - 6); //ignore first 6
  //Serial.println(sizeof menuval);
  for (uint8_t i = 0; i < (sizeof menuval - 6) / 2; i++)
  {
    uint16_t a;
    a = getBackupRegister(i + 1);
    menuval[i * 2 + 6] = (a & 0xff);
    menuval[i * 2 + 1 + 6] = (a >> 8);

    //Serial.println(i);
    //Serial.println(a);
    Serial.println(menuval[i * 2 + 6]);
    Serial.println(menuval[i * 2 + 1 + 6]);
    //Serial.println();
  }


  //initial setup - if "Plus" button is pressed durring reset do a "Factory Resset"
  key_value = analogRead(Analog_Button);
  if (key_value < HH)
    //if (menuval[15] < 1 || menuval[15] > 15 || menuval[16] < 1 || menuval[16] > 15)
  {
    tone(PINSPK, 1000, 100);
    delay(100);
    tone(PINSPK, 1200, 100);
    delay(100);
    tone(PINSPK, 800, 100);
    menuval[6] = 0;   //0=24h, 1=12h
    menuval[7] = 0;   //0=digi, 1=bcd, 2=gps, 3=analog
    menuval[8] = 7;   //alarm hour
    menuval[9] = 30;  //alarm min
    menuval[10] = 0;  //alarm days 0=off, 1=on, 2=workdays
    menuval[11] = 2;  //DST on Week
    menuval[12] = 3;  //DST on Mo
    menuval[13] = 1;  //DST off Week
    menuval[14] = 11; // DST off Mo
    menuval[15] = 5;  //LCD Light Day
    menuval[16] = 1;  //LCD Light Night
    menuval[17] = -4; //Time Zone
    menuval[18] = 0;  //Temp Correction +-5C, 1/10C
    menuval[19] = 17;   //alarm hour
    menuval[20] = 30;  //alarm min
    menuval[21] = 0;  //alarm days 0=off, 1=on, 2=workdays
    
    // reset RTC
    mtt.hour = 12;
    mtt.minute = 01;
    mtt.second = 01;
    mtt.month = 1;
    mtt.day = 1;
    mtt.year = 50;
    tt = rtclock.makeTime(mtt);
    rtclock.setTime(tt);
    
    // write settings to battery backed ram
    backup = true;
    tft.println(F("Reset to factory."));
  }

  // LCD Backlight
  if (menuval[15] < 1)
    menuval[15] = 1; //otherwise we cannot see the display
  if (menuval[16] < 1)
    menuval[16] = 1;

  pwmWrite(PINTFTLED, menuval[15] * 256 * 16);


  // read RTC values from clock chip
  rtclock.breakTime(rtclock.TimeZone(rtclock.getTime(), menuval[17]), mtt);
  menuval[0] = mtt.hour;
  menuval[1] = mtt.minute;
  menuval[2] = mtt.month;
  menuval[3] = mtt.day;
  // Mo = 0
  menuval[4] = mtt.weekday;
  menuval[5] = uint8_t(mtt.year - 30);

  // hours, AM/PM correction
  hours1 = menuval[0];
  if (hours1 > 12)
    hours1 = hours1 - menuval[6] * 12;
  if (hours1 < 1)
    hours1 = hours1 + menuval[6] * 12;

  mode = menuval[7]; //set mode 0 = digi, 1 = bcd

  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);

  delay(500);
  tft.fillScreen(ILI9341_BLACK);

  t1.enable();
  t2.enable();

  //attach interrupt routines
  //secflag = 0;
  //rtclock.attachAlarmInterrupt(AlarmInt);
  rtclock.attachSecondsInterrupt(SecondCount);

  tone(PINSPK, 1000, 100);


  if (menuval[7] == 3)
    drawclock(); //analog clock
}

uint8_t gpsfix;
uint8_t newfix;
uint8_t setgps = 0;
long lat, lon;
char MH[7]; //maidenhead location

void loop()
{
  // taskmanager
  runner.execute();

  //read GPS, set RTC clock if necessary
  while (Serial3.available())
  {
    if (gps.available(gpsPort))
    {
      fix = gps.read();
      if (fix.valid.time)
      {
        if ( setgps == 0) //setgps == 0 at reboot get initial time set
        {
          if (fix.dateTime.seconds == 30)
          { // set the clock from gps only once per min
            mtt.hour = fix.dateTime.hours;
            mtt.minute = fix.dateTime.minutes;
            mtt.second = fix.dateTime.seconds;
            mtt.month = fix.dateTime.month;
            mtt.day = fix.dateTime.date;
            mtt.year = fix.dateTime.year + 30;
            tt = rtclock.makeTime(mtt);
            rtclock.setTime(tt);
            //
            setgps = 1;
#ifdef debug_s
            DEBUG_PORT.print(F("G"));
#endif
          }
        }
        gpsfix = 3; //3 seconds to lose the gps fix
        newfix = 0;
#ifdef debug_s
        //DEBUG_PORT.print(F("g"));
#endif
      }
    }

  }

  //display loop driven by RTC interrupt once per sec
  if (secflag > 0)
  {
    //digitalWrite(PC13, LOW);
    if (mode != 30 && mode != 40)
    {
      //clock display update only if not setup
      myclock();
    }
    secflag = 0;
    if (gpsfix > 0)
      gpsfix--;
  }

  // backup values to rom
  if (backup)
  {
    writeArrayToRom();
    backup = false;
  }
}  //end loop




void writeArrayToRom()
{
  for (uint8_t i = 0; i < (sizeof menuval - 6) / 2 ; i++)
    //for (uint8_t i = 0; i < 6; i++)
  {
    bool result;
    uint16_t a;
    a = (menuval[i * 2 + 1 + 6] << 8) | (menuval[i * 2 + 6] & 0xff);
    result = setBackupRegister(i + 1, a);
    //Serial.println(i + 1);
    //Serial.println(a);
    //Serial.println(menuval[i * 2 + 6]);
    //Serial.println(menuval[i * 2 + 1 + 6]);
    //Serial.println();
  }
}

/*
  void readArrayToRom (int8_t* source, int count)
  {
  for (uint8_t i = 0; (count - 1) / 2; i++) {
    uint16_t a = getBackupRegister(i);
    source[i * 2 + 6] = (a & 0xff);
    source[i * 2 + 1 + 6] = (a >> 8);
    Serial.println(source[i * 2 + 6]);
    Serial.println(source[i * 2 + 1 + 6]);
  }
  }
*/

// from https://github.com/zulns/STM32F1_RTC
// handle battery backed up RAM form RTC

#define PWR_CR PWR_BASE->CR
#define BKP_DR (((volatile uint32_t *)0x40006C00))

inline void enableBackupWrites()
{
  PWR_CR |= PWR_CR_DBP;
}

inline void disableBackupWrites()
{
  PWR_CR &= ~PWR_CR_DBP;
}

uint16_t getBackupRegister(uint8_t idx)
{
  if (1 <= idx && idx <= 10)
    return BKP_DR[idx];
  else
    return 0;
}

bool setBackupRegister(uint8_t idx, uint16_t val)
{
  if (1 <= idx && idx <= 10)
  {
    //enableBackupWrites();
    PWR_CR |= PWR_CR_DBP;

    BKP_DR[idx] = val;

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!for some reason this will crash
    //disableBackupWrites();
    //PWR_CR &= ~PWR_CR_DBP;

    return true;
  }
  else
    return false;
}

void t1Callback()
{ //buttons

  key_value = analogRead(Analog_Button);

  if (key_value > (RR - HH))
  { //10 bit=1024, stm32 is 12 bit 0-4095
    key_press = false;
  }
  else
  {

    //    if (!key_press1)
    //    { //wait for menu routine to be done
    //Serial.print("KV: ");
    //Serial.println(key_value);
    if (key_value > (2 * RR / 3 - HH) && key_value < (2 * RR / 3 + HH) && (!key_press))
    {
      key_press = true;
      key_press2 = millis();
      //key_press1 = true;

      onminusbutton(minusbutton);
    }
    else if (key_value > (RR / 2 - HH) && key_value < (RR / 2 + HH) && (!key_press))
    {
      key_press = true;
      key_press2 = millis();
      //key_press1 = true;

      onmenubutton();
    }
    else if (key_value < HH && (!key_press))
    {
      key_press = true;
      key_press2 = millis();
      //key_press1 = true;

      onminusbutton(plusbutton);
    }
    //    }
  }
}

void t2Callback()
{ //1 s tick
  //Serial.print(F("t2: "));
  //Serial.println(millis() / 1000);

  // menu timeout 1 minute
  long timeinterval = 60000;
  if ((millis() - key_press2) > timeinterval && mode > 29) {
    setexit3();
  }
}

// alarm sound
void t3Callback()
{
  t4.restart(); //reenable and reset task counter
  memcpy_P(&buffer, &alm, 6);
  if (t3.getRunCounter() % 2) {

    tftcprint(ILI9341_RED, 260, 5);
  }
  else {
    tftcprint(ILI9341_BLACK, 260, 5);
  }

  if (t3.isLastIteration()) {
    stopalarm();
  }
  //Serial.print(F("Al: "));
  //Serial.println(millis());
}

void t4Callback()
{
  tone(PINSPK, 1200, 60);
}

void startalarm()
{
  t3.enable();
  t3.restart(); //enable and reset task counter
  alarmon = 1;
  // update display - done by t3Callback
  //memcpy_P(&buffer, &alm, 6);
  //tftcprint(ILI9341_RED, 260, 5);

#ifdef debug_s
  Serial.println(F("Alarm on"));
#endif
}

void stopalarm()
{
  t3.disable(); //stop alarm
  alarmon = 0;
  // clear screen
  memcpy_P(&buffer, &alm, 6);
  tftcprint(ILI9341_BLACK, 260, 5);
  //redraw screen - this will retart alarm
  //seconds--;
  //minutes--;
  //hours--;
  //redraw screen
  if (mode == 0 || mode == 1) //clockdigi and clockbcd only
  {
    pdate(150);
    pyear(260);
  }
  if (mode == 3) {  //analog clock
    pdate(260);
    //pyear();
  }

#ifdef debug_s
  Serial.println(F("Alarm off"));
#endif
}
