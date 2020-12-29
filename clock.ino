//#include "thickline.cpp"
/*
  // Color definitions
  #define ILI9341_BLACK       0x0000  ///<   0,   0,   0
  #define ILI9341_NAVY        0x000F  ///<   0,   0, 123
  #define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
  #define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
  #define ILI9341_MAROON      0x7800  ///< 123,   0,   0
  #define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
  #define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
  #define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
  #define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
  #define ILI9341_BLUE        0x001F  ///<   0,   0, 255
  #define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
  #define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
  #define ILI9341_RED         0xF800  ///< 255,   0,   0
  #define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
  #define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
  #define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
  #define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
  #define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
  #define ILI9341_PINK        0xFC18  ///< 255, 130, 198
*/

void tftcprint(uint16_t color, int y, int textSize)
// print buffer[] center justified on tft
{
  int x;
  //chard are 5X7 with one px space, 6 px width.
  x = (240 - strlen(buffer) * 6 * textSize) / 2;
  //if (x < 0) {
  //  x = 0;
  //};
  /*
    Serial.print (F("center x:"));
    Serial.print (x);
    Serial.print (F(" strlen:"));
    Serial.println (strlen(buffer));
      for (int i = 0; i < 16; i++)
    {
      byte x = buffer[i];
      Serial.print((buffer[i]));
      Serial.print(x);
      Serial.print(F(" "));

    }
    Serial.println("");
  */
  tftprint(color, x, y, textSize);
}


void tftprint(uint16_t color, int x, int y, int textSize)
// print buffer[] on tft, will delete previous text
// https://github.com/adafruit/Adafruit-GFX-Library/issues/16 , have print clear existing text
//To prevent having to clear the text before its drawn,
//like AKA mentions try using the bg color by calling setTextColor(foreground color, background color)
{
  //Serial.println (buffer);
  tft.setCursor(x, y);
  tft.setTextColor(color, ILI9341_BLACK);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(buffer);

}

void clearrow(int y, int textsize)
{
  //strcpy(buffer, "                ");
  if (textsize == 2)memcpy_P(&buffer, &cls, 21);
  else memcpy_P(&buffer, &cls, 15);
  tftcprint(ILI9341_RED, y, textsize);
}

void ptd(uint16_t hh) {  //print temp and date for analog clock

  //cleanup
  clearrow(hh, 2);
  // max string length 20 to keep it one one row for size 2
  //get day of week string
  buffer[0] = '\0';
  memcpy_P (&ast, &dayofweek[menuval[4]], CHARCOUNT);
  strncat(buffer, ast, 9);
  strcat(buffer, " ");

  memcpy_P (&ast, &months[menuval[2] - 1], CHARCOUNT);
  if (strlen(buffer) >= 8) {  //7+1
    strncat(buffer, ast, 3);
  }
  else {
    strncat(buffer, ast, 9);
  }

  strcat(buffer, " ");


  itoa (menuval[3], ast, 10);
  strcat(buffer, ast);

  tftcprint(ILI9341_GREEN, hh, 2);

  //cleanup
  clearrow(hh + 20, 2);


#ifdef debug_s
  //dtostrf(float(menuval[18]) / 10, 5, 1, buffer);
  //Serial.println();
  //Serial.print(F("temp corr "));
  //Serial.println(buffer);
#endif

  // DS18B20 temp sensor
  float temperaturec;
  temperaturec = sensor.getTempC() + float(menuval[18]) / 10; // add temperature correction
  sensor.requestTemperatures();
  float temperaturef;
  temperaturef = (temperaturec * 1.8) + 32;

  dtostrf(temperaturef, 5, 1, buffer);
  strcat(buffer, " F / ");

  dtostrf(temperaturec, 5, 1, ast);
  strcat(buffer, ast);
  strcat(buffer, " C");
  tftcprint(ILI9341_YELLOW, hh + 20, 2);
}

void ptemp()
{
  // DS18B20 temp sensor
  float temperaturec;
  temperaturec = sensor.getTempC() + float(menuval[18]) / 10; // add temperature correction
  sensor.requestTemperatures();

  float temperaturef;
  temperaturef = (temperaturec * 1.8) + 32;

  dtostrf(temperaturef, 5, 1, buffer);
  strcat(buffer, " F");
  tftcprint(ILI9341_YELLOW, 200, 3);
  strcpy(buffer, "o");
  tftprint(ILI9341_YELLOW, 149, 194, 2);

  dtostrf(temperaturec, 5, 1, buffer);
  strcat(buffer, " C");
  tftcprint(ILI9341_YELLOW, 200 + 8 * 4, 3);
  strcpy(buffer, "o");
  tftprint(ILI9341_YELLOW, 149, 194 + 8 * 4, 2);

#ifdef debug_s
  Serial.print(temperaturec); //read registers and display the temperature
  Serial.print(F("C / "));
  Serial.print(temperaturef); //read registers and display the temperature
  Serial.print(F("F"));
#endif
}

// BCD display parameters
#define S 3 //char size 20/2 or 30/3
#define P 30 //spacing

#define C ILI9341_GREEN //color on
#define O ILI9341_MAROON //color off

#define X 10 //X offset
#define Y 5 //Y offset

void bcdp(uint8_t val, uint8_t x )
{
  for (int i = 0; i < 4; i++) {
    if (val & (0b1000 >> i)) {
      strcpy(buffer, "\""); tftprint(C, x * P + X, P * i + Y, S);
    }
    else {
      strcpy(buffer, "\""); tftprint(O, x * P + X, P * i + Y, S);
    }
  }
}


void pyear(uint16_t hh)  //default 260
//DayWeekNumber: Calculate day of year and week of year
// this routine does come from http://arduino.joergeli.de
/*
  menuval[0] hour 0-23
  menuval[1] min 0 -59
  menuval[2] month 1-12
  menuval[3] dayofmonth 1-31
  menuval[4] dayofweek 0-6, 0=Monday
  expected Su=0
  menuval[5] year last 2 digits 19XX
*/
{
  int w;
  if (menuval[4] == 6 ) {
    w = 0;
  }
  else {
    w = menuval[4] + 1;
  }

  int days[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  int DN, WN;
  // Number of days at the beginning of the month in a not leap year.
  //Start to calculate the number of day
  if (menuval[2] == 1 || menuval[2] == 2) {
    DN = days[(menuval[2] - 1)] + menuval[3]; //for any type of year, it calculate the number of days for January or february
  }                                                          // Now, try to calculate for the other months
  else if (((menuval[5] + 2000) % 4 == 0 && (menuval[5] + 2000) % 100 != 0) ||  (menuval[5] + 2000) % 400 == 0) { //those are the conditions to have a leap year
    DN = days[(menuval[2] - 1)] + menuval[3] + 1;                              // if leap year, calculate in the same way but increasing one day
  }
  else {                                                     //if not a leap year, calculate in the normal way, such as January or Febr
    DN = days[(menuval[2] - 1)] + menuval[3];
  }
  // Now start to calculate Week number
  if (w == 0) {
    WN = (DN - 7 + 10) / 7;                                  //if it is sunday (time library returns 0)
  }
  else {
    WN = (DN - w + 10) / 7;                                  // for the other days of week
  }

  itoa ((menuval[5] + 2000), buffer, 10);
  memcpy_P (&ast, wday[0], 7);
  strcat(buffer, ast);
  itoa (WN, ast, 10);
  strcat(buffer, ast);
  tftcprint(ILI9341_GREEN, hh, 2);
  memcpy_P (&buffer, wday[1], 7);
  //strcpy(buffer, " Day ");
  itoa (DN, ast, 10);
  strcat(buffer, ast);
  tftcprint(ILI9341_GREEN, hh + 20, 2);
}


void pdate(uint16_t hh)  //default 150
{
  //cleanup
  clearrow(hh, 2);
  //get day of week string
  memcpy_P (&buffer, &dayofweek[menuval[4]], CHARCOUNT);
  tftcprint(ILI9341_GREEN, hh, 2);

#ifdef debug_s
  Serial.print(buffer);
  Serial.print(F(" "));
#endif
  //cleanup
  clearrow(hh + 20, 2);
  //get month string
  //tftcprint(ILI9341_GREEN, 170, 2);
  memcpy_P (&buffer, &months[menuval[2] - 1], CHARCOUNT);
  itoa (menuval[3], ast, 10);
  strcat(buffer, " ");
  strcat(buffer, ast);
  tftcprint(ILI9341_GREEN, hh + 20, 2);

#ifdef debug_s
  Serial.println(buffer);
#endif
}


void clockbcd()
{
  bcdp(seconds / 10, 6);
  bcdp(seconds % 10, 7);

  // hours
  bcdp(hours1 / 10, 0);
  bcdp(hours1 % 10, 1);

  // min
  bcdp(menuval[1] / 10, 3);
  bcdp(menuval[1] % 10, 4);

#ifdef debug_s
  Serial.println(hours1);
  Serial.print(F("%"));
  Serial.print(menuval[1]);
  Serial.print(F(" "));
#endif
}

void leaditoa (int8_t val1) {
  itoa (val1, ast, 10);
  if (val1 < 10) {
    strcat(buffer, "0");
    strcat(buffer, ast);
  }
  else {
    strcat(buffer, ast);
  }
}

void ptimed (uint16_t hh)
{
  //print hour:min
  strcpy(buffer, "");
  //leaditoa(hours1);
  itoa (hours1, ast, 10);
  if (hours1 < 10) {
    strcat(buffer, " ");
    strcat(buffer, ast);
  }
  else {
    strcat(buffer, ast);
  }


  // 12 hours menuval[6] 0=24h 1=12h
  if (menuval[6] == 1) {
    if (menuval[0] > 11) {
      // print hours in different color
      tftprint(ILI9341_RED, (240 - 5 * 6 * 8) / 2 + 13, hh, 8);
      //PM
    }
    else {
      // print hours in different color
      tftprint(ILI9341_YELLOW, (240 - 5 * 6 * 8) / 2 + 13, hh, 8);
      //AM
    }
  }
  else {
    // print hours in different color
    //x = (240 - strlen(buffer) * 6 * textSize) / 2;
    tftprint(ILI9341_WHITE, (240 - 5 * 6 * 8) / 2 + 13, hh, 8);
  }

  strcpy(buffer, ":");
  tftprint(ILI9341_WHITE, 100, hh, 7);
  // minutes
  strcpy(buffer, "");
  leaditoa(menuval[1]);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  //?tft.setTextSize(7);
  //tft.print(buffer);
  tftprint(ILI9341_WHITE, 130, hh, 8);

}


void clockdigi()
{
  ptimed(50);
}



void gpsclock() {
  ptimed(5);

  //UTC on internal clock or GPS time when available
  if (fix.valid.time && gpsfix > 0) {
    strcpy(buffer, "GPS ");
    leaditoa(fix.dateTime.hours);
    strcat(buffer, ":");
    // minutes
    leaditoa(fix.dateTime.minutes);
    tftcprint(ILI9341_YELLOW, 70, 4);
  }
  else {
    rtclock.breakTime(rtclock.TimeZone(rtclock.getTime(), 0), mtt);
    strcpy(buffer, "UTC ");
    leaditoa(mtt.hour);
    strcat(buffer, ":");
    leaditoa(mtt.minute);
    tftcprint(ILI9341_GREEN, 70, 4);
  }


  if (fix.valid.time && gpsfix > 0) {
    strcpy(buffer, "");
    leaditoa(fix.dateTime.month);
    strcat(buffer, "/");
    leaditoa(fix.dateTime.date);
    strcat(buffer, "/");
    itoa ((fix.dateTime.year + 2000), ast, 10);
    strcat(buffer, ast);
    tftcprint(ILI9341_YELLOW, 105, 3);
  }
  else {
    //strcpy(buffer, "no fix ");
    strcpy(buffer, "");
    leaditoa(mtt.month);
    strcat(buffer, "/");
    leaditoa(mtt.day);
    strcat(buffer, "/");
    itoa ((mtt.year - 30 + 2000), ast, 10);
    strcat(buffer, ast);
    tftcprint(ILI9341_GREEN, 105, 3);
  }

  if (fix.valid.location && gpsfix > 0) {
    lat  = fix.latitudeL();
    lon  = fix.longitudeL();
    strcpy(buffer, "Lat:");
    itoa (lat, ast, 10);
    //??ast=fix.latitude();
    strcat(buffer, ast);
  }
  else strcpy(buffer, ("Lat:---------"));
  tftcprint(ILI9341_WHITE, 135, 2);

  if (fix.valid.location && gpsfix > 0) {
    strcpy(buffer, "Lon:");
    itoa (lon, ast, 10);
    strcat(buffer, ast);
  }
  else strcpy(buffer, ("Lon:--------- "));
  tftcprint(ILI9341_WHITE, 155, 2);

  if (fix.valid.location && gpsfix > 0) {
    // QTH locator / MaidenHead
    //this will destroy the content of lat and lon nad update MH
    //From:https://github.com/TypeFaster2/Maidenhead/blob/master/GPS_V17_Small.ino
    // keep only 5 decimals
    lon = lon / 100 + 18000000;                          // Step 1
    lat = lat / 100 +  9000000;                          // Adjust so Locn AA is at the pole
    strncpy(MH, "AA00aa", 7);
    MH[0] +=  lon / 2000000;                       // Field
    MH[1] +=  lat / 1000000;
    MH[2] += (lon % 2000000) / 200000;             // Square
    MH[3] += (lat % 1000000) / 100000;
    MH[4] += (lon %  200000) /   8333;             // Subsquare .08333 is   5/60
    MH[5] += (lat %  100000) /   4166;             //           .04166 is 2.5/60
    strcpy(buffer, "QTH:");
    strcat(buffer, MH);
  }
  else strcpy(buffer, "QTH:------");
  tftcprint(ILI9341_YELLOW, 175, 3);

  if (fix.valid.altitude && gpsfix > 0) {
    strcpy(buffer, "Alt:");
    itoa (fix.altitude_cm() / 100, ast, 10);
    strcat(buffer, ast);
    /*
        if (fix.altitude_cm() < hmin) {
          hmin = fix.altitude_cm();
        }

        if (hmin == 0) {
          hmin = fix.altitude_cm();
        }

        if (fix.altitude_cm() > hmax) {
          hmax = fix.altitude_cm();
        }

        strcat(buffer, " Dif:");
        itoa (hmax - hmin, ast, 10);
        strcat(buffer, ast);
    */
    strcat(buffer, " Hdop:");
    itoa (fix.hdop / 10, ast, 10);
    strcat(buffer, ast);
    strcat(buffer, "  ");

  }
  else strcpy(buffer, "                    ");
  tftcprint(ILI9341_WHITE, 205, 2);

  strcpy(buffer, "Sat:");
  itoa (fix.satellites, ast, 10);
  strcat(buffer, ast);
  itoa (fix.satellites, ast, 10);
  strcat(buffer, " Status:");
  itoa (fix.status, ast, 10);
  strcat(buffer, ast);
  strcat(buffer, "  ");
  tftcprint(ILI9341_WHITE, 225, 2);
}


// An analog clock
// Gilchrist 6/2/2014 1.0
// Updated by Alan Senior 18/1/2015

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
//uint16_t osx = 120, osy = 120, omx = 130, omy = 130, ohx = 130, ohy = 130; // Saved H, M, S x & y coords
uint16_t x00 = 0, x11 = 0, y00 = 0, y11 = 0;
boolean initial = 1;

void drawclock() {
  tft.setTextColor(ILI9341_WHITE, ILI9341_DARKGREY);  // Adding a background colour erases previous text automatically

  // Draw clock face
  tft.fillCircle(120, 120, 118, ILI9341_GREEN);
  //tft.fillCircle(120, 120, 110, ILI9341_BLACK);
  tft.fillCircle(120, 120, 115, ILI9341_BLACK);
  // Draw 12 lines
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x00 = sx * 114 + 120;
    y00 = sy * 114 + 120;
    x11 = sx * 100 + 120;
    y11 = sy * 100 + 120;

    //tft.drawLine(x00, y00, x11, y11, ILI9341_GREEN);
    drawThickLine(x00, y00, x11, y11, 3, 0, ILI9341_WHITE);

    // Draw main quadrant dots
    if (i == 0 || i == 180) drawThickLine(x00, y00, x11, y11, 7, 0, ILI9341_WHITE); //tft.fillCircle(x00, y00, 2, ILI9341_WHITE);
    if (i == 90 || i == 270) drawThickLine(x00, y00, x11, y11, 7, 0, ILI9341_WHITE); //tft.fillCircle(x00, y00, 2, ILI9341_WHITE);
  }

  // Draw 60 dots
  for (int i = 0; i < 360; i += 6) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x00 = sx * 102 + 120;
    y00 = sy * 102 + 120;
    // Draw minute markers
    tft.drawPixel(x00, y00, ILI9341_WHITE);

    // Draw main quadrant dots
    //if (i == 0 || i == 180) drawThickLine(x00, y00, x11, y11, 7, 0, ILI9341_WHITE); //tft.fillCircle(x00, y00, 2, ILI9341_WHITE);
    //if (i == 90 || i == 270) drawThickLine(x00, y00, x11, y11, 7, 0, ILI9341_WHITE); //tft.fillCircle(x00, y00, 2, ILI9341_WHITE);
  }

  tft.fillCircle(120, 121, 3, ILI9341_WHITE);

  // Draw text at position 120,260 using fonts 4
  // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
  // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
  //tft.drawCentreString("Time flies",120,260,4);

}


void analogclock() {

#ifdef debug_s
  //Serial.print(menuval[0]);
  //Serial.print(F(":"));
  //Serial.print(menuval[1]);
  //Serial.print(F("."));
  //Serial.println(seconds);
#endif

  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = seconds * 6;                // 0-59 -> 0-354
  mdeg = menuval[1] * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
  hdeg = menuval[0] * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
  hx = cos((hdeg - 90) * 0.0174532925);
  hy = sin((hdeg - 90) * 0.0174532925);
  mx = cos((mdeg - 90) * 0.0174532925);
  my = sin((mdeg - 90) * 0.0174532925);
  sx = cos((sdeg - 90) * 0.0174532925);
  sy = sin((sdeg - 90) * 0.0174532925);

  if (seconds == 0 || initial) {
    initial = 0;
    // Erase hour and minute hand positions every minute
    //void drawThickLine(unsigned int aXStart, unsigned int aYStart, unsigned int aXEnd, unsigned int aYEnd, unsigned int aThickness,uint8_t aThicknessMode, uint16_t aColor)

    //tft.drawLine(ohx, ohy, 120, 121, ILI9341_BLACK);
    drawThickLine(ohx, ohy, 120, 121, 7, 0, ILI9341_BLACK);
    ohx = hx * 62 + 121;
    ohy = hy * 62 + 121;

    //tft.drawLine(omx, omy, 120, 121, ILI9341_BLACK);
    drawThickLine(omx, omy, 120, 121, 5, 0, ILI9341_BLACK);
    omx = mx * 84 + 120;
    omy = my * 84 + 121;
  }

  // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
  tft.drawLine(osx, osy, 120, 121, ILI9341_BLACK);
  osx = sx * 90 + 121;
  osy = sy * 90 + 121;
  //tft.drawLine(osx, osy, 120, 121, ILI9341_RED);

  //tft.drawLine(ohx, ohy, 120, 121, ILI9341_WHITE);
  drawThickLine(ohx, ohy, 120, 121, 7, 0, ILI9341_YELLOW);
  //tft.drawLine(omx, omy, 120, 121, ILI9341_WHITE);
  drawThickLine(omx, omy, 120, 121, 5, 0, ILI9341_WHITE);

  tft.drawLine(osx, osy, 120, 121, ILI9341_RED);

  //tft.fillCircle(120, 121, 3, ILI9341_RED);
  tft.fillCircle(120, 121, 3, ILI9341_RED);
}

void myclock()
{
  rtclock.breakTime(rtclock.TimeZone(rtclock.getTime(), menuval[17]), mtt);
  seconds = mtt.second;
  menuval[0] = mtt.hour;
  menuval[1] = mtt.minute;
  menuval[2] = mtt.month;
  menuval[3] = mtt.day;
  menuval[4] = mtt.weekday;
  menuval[5] = uint8_t(mtt.year - 30);

  hours1 = menuval[0];  //AM/PM hour correction
  if (hours1 > 12) hours1 = hours1 - menuval[6] * 12;
  if (hours1 < 1) hours1 = hours1 + menuval[6] * 12;

  // update bcdclock once per sec
  if (mode == 1) clockbcd();
  if (mode == 2) gpsclock();
  if (mode == 3) analogclock();

  //display status on lcd last line
  if (mode == 0 || mode == 2 || mode == 3) { // only in clockdigi, clockgps
    itoa (seconds, ast, 10);
    if (seconds < 10) {
      strcpy(buffer, "0");
      strcat(buffer, ast);
    }
    else {
      strcpy(buffer, ast);
    }

    //show gps fix as GPS
    if (gpsfix > 0) {
      strcat(buffer, " GPS");
    }
    else {
      strcat(buffer, "    ");
    }

    //show Alarm as A1A2
    //menuval[10]
    //menuval[21] alarm days 0=off, 1=on, 2=workdays

    //strcat(buffer, " AL");
    if (menuval[10] > 0) {
      strcat(buffer, " A1");
    }
    else {
      strcat(buffer, " ");
    }
    if (menuval[21] > 0) {
      strcat(buffer, "A2");
    }
    else {
      strcat(buffer, " ");
    }

    //debug
    //itoa (key_value, ast, 10);
    //strcat(buffer, ast);

    tftcprint(ILI9341_RED, 300, 2);
  }

#ifdef debug_s
  Serial.print(F(">"));
#endif

  // update once per min
  if (minutes != mtt.minute) {
    minutes = mtt.minute;

#ifdef debug_s
    Serial.print(menuval[0]);
    Serial.print(F(":"));
    Serial.println(menuval[1]);
#endif

    // update clockdigi
    if (mode == 0) clockdigi();

    if (mode == 0 || mode == 1) //clockdigi or clockbcd
    {
      ptemp(); // show temp once per min
    }

    if (mode == 3 || mode == 2) { //analog clock or gps clock

      ptd(260);
    }

    // check for alarm
    //menuval[8] alarm hour
    //menuval[9] alarm min
    //menuval[10] alarm days 0=off, 1=on, 2=workdays
    //menuval[19] alarm hour
    //menuval[20] alarm min
    //menuval[21] alarm days 0=off, 1=on, 2=workdays
    /*Mo=0 menuval[4] */
    if (menuval[8] == menuval[0] && menuval[9] == menuval[1] && menuval[10] > 0) {
      // alarm on, check day
      if (menuval[10] == 1) { //each day
        startalarm();
      }
      else if (menuval[10] == 2 && menuval[4] < 5) { //weekday menuval[4],mo=0, sun=6
        startalarm();
      }
    }
    
    if (menuval[19] == menuval[0] && menuval[20] == menuval[1] && menuval[21] > 0) {
      // alarm on, check day
      if (menuval[21] == 1) { //each day
        startalarm();
      }
      else if (menuval[21] == 2 && menuval[4] < 5) { //weekday menuval[4],mo=0, sun=6
        startalarm();
      }
    }

    // once per hour
    if (hours != mtt.hour) {
      hours = mtt.hour;
      Serial.print(F("GPSH"));

      // set time from gps once per hour
      setgps = 0;

      if (mode == 0 || mode == 1) //clockdigi and clockbcd only
      {
        pdate(150);
        pyear(260);
      }
      if (mode == 3) {  //analog clock
        //pdate(260);
        //pyear();
      }
      // set LCD backlight for day/night
      if (menuval[0] == STARTNIGHT)
        pwmWrite(PINTFTLED, menuval[16] * 256 * 16);

      if (menuval[0] == STARTDAY)
        pwmWrite(PINTFTLED, menuval[15] * 256 * 16);

#ifdef debug_s
      Serial.println();
      Serial.print(menuval[0]);
      Serial.print(F(":"));
      Serial.print(menuval[1]);
      Serial.print(F(" H "));
      Serial.print(menuval[2]);
      Serial.print(F(" "));
      Serial.print(menuval[3]);
      Serial.print(F(" wd"));
      Serial.print(menuval[4]);
      Serial.print(F(" y"));
      Serial.println(menuval[5]);
#endif


      // check for DST
      //DST on
      if (menuval[12] == menuval[2]) //month
      {
#ifdef debug_s
        Serial.print(F("DstOn "));
        Serial.print(menuval[14]);
        Serial.print(F(" "));
        Serial.print(((menuval[3] + 1) / 7 + 1));
        Serial.print(F(" "));
        Serial.print(menuval[0]);
        Serial.print(F(" "));
        Serial.println(menuval[4]);  //mo weekno hour dayno
#endif
        if (((menuval[3] + 1) / 7 + 1) == menuval[11] && menuval[0] == 2 && menuval[4] == 6) // week number, 2AM, Sunday=day 6
        //if (((menuval[3] + 1) / 7 + 1) == menuval[11] && menuval[0] == 2 && menuval[4] == 6) // week numer, 2AM, sunday = day 6
        {
          //DST on + 1 hour, change timezone
          if (setdst != 1 ) {
            Serial.println(F("timezone +=1"));
            menuval[17] += 1;
            writeArrayToRom();
            setdst = 1;
          }
        }
      }

      //DST off
      if (menuval[14] == menuval[2]) //month
      {
#ifdef debug_s
        Serial.print(F("DstOff "));
        Serial.print(menuval[14]);
        Serial.print(F(" "));
        Serial.print(((menuval[3] + 1) / 7 + 1));
        Serial.print(F(" "));
        Serial.print(menuval[0]);
        Serial.print(F(" "));
        Serial.println(menuval[4]); //mo weekno hour dayno
#endif
        if (((menuval[3] + 1) / 7 + 1) == menuval[13] && menuval[0] == 2 && menuval[4] == 6) // week numer, 2AM, sunday = day 6
        //if (((menuval[3] + 1) / 7 + 1) == menuval[13] && menuval[0] == 2 && menuval[4] == 5) // week numer, 2AM, sunday = day 6
        {
          //DST off -1 hour, change timezone
          if (setdst != 2 ) {
            Serial.println(F("timezone -=1"));
            menuval[17] -= 1;
            writeArrayToRom();
            setdst = 2;
          }
        }
      }
    }
  }
}



