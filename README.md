TFT LCD STM 32 Bluepill Clock GPS disciplined

Is a magic clock :). You plug it in and will always show the right time.

This Clock is using a 320X240 ILI9341 tft lcd connected over SPI.
Also the Blupill does have a 32Khz crystal for RTC and a backup CR2032 battery will keep the clock running and the battery backed up RAM with all the settings up and running even when the power is off. 
A GPS module is used to set the clock and display the QTH locator. 

BOM:
Bluepill module with 128K flash, even a Chinese clone SRM32 will work.
320X240 ILI9341 TFT LCD any size with a 14 pin connector, tested whit 2.8' and 3.2' displays.
Neo 6M or 8M GPS module with antenna/ external antenna. Depending on the local conditions and building you may need an external antenna to get a good GPS fix. I will suggest a neo-8M with a full-size patch antenna or external antenna if that will not work inside. DS18B20 temperature sensor with F/C temperature display.
3 push buttons to control the clock and a piezo buzzer for alarm. See the attached schematic.

The clock has a menu with many options in order to set the time, date, 2 alarms, DST settings and multiple clock faces - Analog, Digital, BCD, GPS details - see attached pictures, display back-light will automatically adjust day/night.

The menu of this clock is designed around a struct in PROGMEM in order to use less RAM, for those interested this can be reused as a menu for other projects. It was an interesting exercise to create and is working quite nice with the three buttons: "-", "Setup" and "+". Can be easily extended as necessary.

All the libraries used are in a zip file (unzip under your arduino / libraries folder), the font file form Adafruit_GFX is modified “ char is replaced by a full circle character used by the BCD clock display and the second half of the table is removed in order to save space.

There is a precompiled bin file that you can use to program the bluepill - Generic STM32F103C series, STM32F103CB (20k RAM, 128k Flash) 48Mhz (slow - with USB) or 72 Mhz. Optimize -O2. there are many ways to program STM32 but the easier way is to use an ST-Link.
For the brave ones out there interested to make this better: install all the files in Arduino and compile the code - use Additional Boards manger URL:http://dan.drown.org/stm32duino/package_STM32duino_index.json to install the Bluepill specific files.






