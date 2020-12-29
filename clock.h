/*
  clock menu
*/


// we define all the strings in PROGMEM

// max# of char in the array+1
const int CHARCOUNT = 10;

/* Mo=0 Tu=1 We=2 Th=3 Fr=4 Sa=5 Su=6 */
const char dayofweek [7] [CHARCOUNT] PROGMEM = {
  { "Monday" },
  { "Tuesday" },
  { "Wednesday" },
  { "Thursday" },
  { "Friday" },
  { "Saturday" },
  { "Sunday" }
};

const char months [12] [CHARCOUNT] PROGMEM = {
  { "January" },
  { "February" },
  { "March" },
  { "April" },
  { "May" },
  { "June" },
  { "July" },
  { "August" },
  { "September" },
  { "October" },
  { "November" },
  { "December" }
};

const char h1224 [2] [CHARCOUNT] PROGMEM = {
  { "24 hour" },
  { "12 hour" }
};

const char clmode [4] [CHARCOUNT] PROGMEM = {
  { "Digi Clk" },
  { "BCD Clk" },
  { "GPS Clk" },
  { "Analog" }
};

const char cls[21] PROGMEM = "                    "; //20 chars
const char alm[5 + 1] PROGMEM = "ALARM";

const char apm [2] [2 + 1] PROGMEM = {
  { "AM" },
  { "PM" }
};

const char wday [2] [6 + 1] PROGMEM = {
  { " Week " },
  { " Day " }
};

const char alarmdays1 [3] [CHARCOUNT] PROGMEM = {
  { "AlarmOff" },
  { "Alarm On" },
  { "Workdays" }
};


// if you want to understand how this does work read
// https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs

const int MENUCOUNT = 23;
int8_t menuval[MENUCOUNT];   // values for menu items

struct MenuRecord
{
  int8_t start;  // low value
  int8_t count;  // high value
  char item[12 + 1]; // menu choice text to display
  const char (*values)[CHARCOUNT];
  void (*set)();  // setup routine, call if we press set for an item
};

//menu routines
void settime1();
void settimemin();
void setexit3();
void exitmenu2();
void setlcd();
void setlcdn();

const MenuRecord menu[MENUCOUNT] PROGMEM = {
  { 0, 23, "Time hour", NULL , settime1 }
  , { 0, 59, "Time min", NULL , settimemin }
  , { 1, 12, "Month", months , settime1 }
  , { 1, 31, "Date", NULL, settime1 }
  , { 1, 7, "Day of week", dayofweek, settime1 }
  , { 20, 127, "Year", NULL, settime1 }
  , { 0, 1, "Clock 12/24H", h1224, setexit3 }
  , { 0, 3, "Clock mode", clmode, setexit3 }
  , { 0, 23, "Alarm1 hour", NULL, exitmenu2 }
  , { 0, 59, "Alarm1 min", NULL, exitmenu2 }
  , { 0, 2, "Alarm1 Days", alarmdays1 , exitmenu2 }
  , { 0, 5, "DSTonWeek", NULL , exitmenu2 }
  , { 0, 12, "DSTonMo", NULL , exitmenu2 }
  , { 0, 5, "DSToffWeek", NULL , exitmenu2 }
  , { 0, 12, "DSToffMo", NULL , exitmenu2 }
  , { 1, 16, "LCD Day", NULL , setlcd }
  , { 1, 16, "LCD Night", NULL , setlcdn }
  , { -12, 12, "Time Zone", NULL , exitmenu2 }
  , { -50, 50, "Temp Corr/10", NULL , exitmenu2 }
  , { 0, 23, "Alarm2 hour", NULL, exitmenu2 }
  , { 0, 59, "Alarm2 min", NULL, exitmenu2 }
  , { 0, 2, "Alarm2 Days", alarmdays1 , exitmenu2 }  
  , { 0, 1, "Exit Setup", NULL, setexit3 }
};


/*
  menuval[0] hour 0-23
  menuval[1] min 0 -59
  menuval[2] month 1-12
  menuval[3] dayofmonth 1-31
  menuval[4] dayofweek 1-7, 1=Monday
  menuval[5] year last 2 digits 19XX
  menuval[6] 0=24h, 1=12h
  menuval[7] 0=digi, 1=bcd
  menuval[8] alarm hour
  menuval[9] alarm min
  menuval[10] alarm days 0=off, 1=on, 2=workdays
  menuval[11] DST on Week
  menuval[12] DST on Mo
  menuval[13] DST off Week
  menuval[14] DST off Mo
  menuval[15] LCD Light Day
  menuval[16] LCD Light Night
  menuval[17] Time Zone
  menuval[18] Temp Comp 1/10 C//+-5C
  menuval[19] alarm hour
  menuval[20] alarm min
  menuval[21] alarm days 0=off, 1=on, 2=workdays
*/

