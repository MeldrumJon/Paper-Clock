#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <DS1337RTC.h>

void clock_init(); 
void clock_intrpt_en();
void clock_intrpt_dis();
#define clock_intrpt_ack() RTC.resetAlarms()

tmElements_t* clock_read();
void clock_set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, 
        uint8_t minute, uint8_t second); 

int8_t clock_12to24(int8_t hfmt12, uint8_t ispm);
int8_t clock_24to12(int8_t hour);
int8_t clock_dayofweek(int16_t year, int8_t month, int8_t day);

extern volatile uint_fast8_t clk_intrpt_flag;

#endif
