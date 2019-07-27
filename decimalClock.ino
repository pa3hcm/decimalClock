/*
  Project:   decimalClock
  Source:    http://github.com/pa3hcm/decimalClock
  Authors:   Ernest Neijenhuis
  Copyright: (c) 2019 Ernest Neijenhuis PA3HCM
*/


/////////////////////////////////////////////////////////////////////////
// Constants

// Pin labels
const int P_RTC_IO    =  9; // Connected to DS1302 RTC module, pin DAT
const int P_RTC_CLK   =  8; // Connected to DS1302 RTC module, pin CLK
const int P_RTC_CE    = 10; // Connected to DS1302 RTC module, pin RST


// LCD settings
const int LCD_ROWS    = 4;    // Number of display rows
const int LCD_COLS    = 20;   // Number of display columns
const int LCD_I2CADDR = 0x27; // I2C address, in most cases this is 0x27


/////////////////////////////////////////////////////////////////////////
// Libraries

// LiquidCrystal_I2C source & info:
// https://github.com/johnrickman/LiquidCrystal_I2C
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(LCD_I2CADDR, LCD_COLS, LCD_ROWS);

// Rtc source & info:
// https://github.com/Makuna/Rtc
// Rtc with DS1302 module info:
// https://github.com/Makuna/Rtc/wiki/RtcDS1302-object
#include <ThreeWire.h> 
#include <RtcDS1302.h>
ThreeWire myWire(P_RTC_IO, P_RTC_CLK, P_RTC_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
#define countof(a) (sizeof(a) / sizeof(a[0]))


/////////////////////////////////////////////////////////////////////////
// setup() routine

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(115200);

  // RTC initialization, health checks, etc...
  // Copied from the DS1302 example that comes with the Rtc library
  Rtc.Begin();
  RtcDateTime compileDateTime = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compileDateTime);
  Serial.println();
  if (!Rtc.IsDateTimeValid()) {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compileDateTime);
  }
  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compileDateTime) {
    Serial.println("RTC is older than compile time! (Updating DateTime)");
    Rtc.SetDateTime(compileDateTime);
  } else if (now > compileDateTime) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compileDateTime) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}


/////////////////////////////////////////////////////////////////////////
// Main loop() routine

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("=== decimalClock ===");

  RtcDateTime now = Rtc.GetDateTime();
  
  char datestring[12];
  snprintf_P(datestring, countof(datestring), PSTR("%02u-%02u-%04u "),
    now.Day(), now.Month(), now.Year() );
  lcd.setCursor(1, 2);
  lcd.print(datestring);

  char timestring[9];
  float decimalTime = (now.Hour()*3600.0 + now.Minute()*60.0 + now.Second()*1.0 ) / 8640.0;
  dtostrf(decimalTime, 7, 4, timestring);
  lcd.setCursor(12, 2);
  lcd.print(timestring);
  delay(200);
}


/////////////////////////////////////////////////////////////////////////
// printDateTime(), copied from one of the Rtc library examples

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];
  snprintf_P(datestring, 
    countof(datestring),
    PSTR("%02u-%02u-%04u %02u:%02u:%02u"),
    dt.Day(),
    dt.Month(),
    dt.Year(),
    dt.Hour(),
    dt.Minute(),
    dt.Second()
  );
  Serial.print(datestring);
}
