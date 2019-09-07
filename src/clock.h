#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <DS1337RTC.h>

void clock_init(); 
void clock_intrpt_en();
void clock_intrpt_dis();
void clock_intrpt_ack();

void clock_set(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, 
        uint8_t minute, uint8_t second); 

// time_t clock_read();
#define clock_read() RTC.get(CLOCK_ADDRESS)
#define clock_intrpt_ack() RTC.resetAlarms();

extern volatile uint_fast8_t clk_intrpt_flag;

uint8_t clock_12to24(uint8_t hfmt12, uint8_t ispm);

#endif
