#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <DS1337RTC.h>

void clock_init(); 

void clock_set(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, 
        uint8_t minute, uint8_t second); 

#define clock_read() RTC.get(CLOCK_ADDRESS)

#endif
