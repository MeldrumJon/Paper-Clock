#include "clock.h"
#include <Wire.h>

void clock_init() {
   Wire.begin();

   // TODO: set up interrupt
}

void clock_set(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, 
        uint8_t minute, uint8_t second) {
    tmElements_t tmSet;
    tmSet.Year = y2kYearToTm(year);
    tmSet.Month = month;
    tmSet.Day = day;
    tmSet.Hour = hour;
    tmSet.Minute = minute;
    tmSet.Second = second;
    RTC.set(makeTime(tmSet), CLOCK_ADDRESS); // set the clock
}

// Defined in header:
// time_t clock_read();
