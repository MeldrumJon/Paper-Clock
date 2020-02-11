#include "clock.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <Wire.h>
#include <TimeLib.h>

#define CLK_INT_DMASK 0x04
#define INT0_EN_MASK 0x01

#define MIN_INTRPT_SEQ_LEN 4
const uint8_t MIN_INTRPT_SEQ[MIN_INTRPT_SEQ_LEN] = {
    ALARM2_ADDRESS,
    0x80,
    0x80,
    0x80
};

static tmElements_t timeElems;

volatile uint_fast8_t clk_intrpt_flag = 0;

ISR(INT0_vect, ISR_BLOCK) {
	clk_intrpt_flag = 1; // Do something with this flag to turn back on timer2
}

void clock_init() {
    Wire.begin();

    RTC.interruptSelect(INTB);
    // Set up interrupt every minute at :00
    Wire.beginTransmission(DS1337_CTRL_ID);
    for (uint8_t i = 0; i < MIN_INTRPT_SEQ_LEN; i++) {
        Wire.write(MIN_INTRPT_SEQ[i]);
    }
    Wire.endTransmission();

	DDRD &= ~(CLK_INT_DMASK); // Set pin as input
	PORTD |= CLK_INT_DMASK; // Activate pull-up
    return;
}

void clock_intrpt_en() {
    RTC.enableAlarm(ALARM2_ADDRESS);
	cli();
	EICRA |= 0x02; // Falling edge interrupt
	EIMSK |= INT0_EN_MASK; // Set interrupt mask on INT0
	sei();
    RTC.resetAlarms();
    return;
}

void clock_intrpt_dis() {
    RTC.disableAlarm(ALARM2_ADDRESS);
	cli();
	EIMSK &= ~(INT0_EN_MASK); // Disable interrupt
	sei();
    return;
}

// Defined in header:
// void clock_intrpt_ack()

tmElements_t* clock_read() {
    RTC.read(timeElems, CLOCK_ADDRESS); // timeElems passed by reference
    return &timeElems;
}

void clock_set(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, 
        uint8_t minute, uint8_t second) {
    timeElems.Year = CalendarYrToTm(year);
    timeElems.Month = month;
    timeElems.Day = day;
    timeElems.Hour = hour;
    timeElems.Minute = minute;
    timeElems.Second = second;
    RTC.stopClock();
    RTC.write(timeElems, CLOCK_ADDRESS); // timeElems passed by reference
    RTC.startClock();
    return;
}

int8_t clock_12to24(int8_t hfmt12, uint8_t ispm) {
    if (!ispm && hfmt12 == 12) { return 0; } // 12am
    else if (!ispm) { return hfmt12; } // am
    else if (ispm && hfmt12 == 12) { return hfmt12; } // 12pm
    else { return hfmt12+12; } // pm
}

int8_t clock_24to12(int8_t hour) {
    if (hour == 0) { return 12; } // 12am
    else if (hour > 12) { return (hour - 12); } // pm
    else { return hour; } // am
}
int8_t clock_dayofweek(int16_t year, int8_t month, int8_t day) {
    static const int8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    static int8_t dow = 0;
    static int16_t last_year = 0;
    static int8_t last_month = 0;
    static int8_t last_day = 0;

    if (day != last_day || month != last_month || year != last_year) {
        year -= (month < 3) ? 1 : 0;
        dow = (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
        dow += 1; // Return 1-7 instead of 0-6

        last_year = year;
        last_month = month;
        last_day = day;
    } // return cached value if day, month, year same as last call
    return dow;
}

