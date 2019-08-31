#include "clock.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <Wire.h>

#define CLK_INT_DMASK 0x04
#define INT0_EN_MASK 0x01

#define MIN_INTRPT_SEQ_LEN 4
const uint8_t MIN_INTRPT_SEQ[MIN_INTRPT_SEQ_LEN] = {
    ALARM2_ADDRESS,
    0x80,
    0x80,
    0x80
};

volatile uint_fast8_t clk_intrpt_flag = 0;

ISR(INT0_vect, ISR_BLOCK) {
	clk_intrpt_flag = !(0); // Do something with this flag to turn back on timer2
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
}

void clock_intrpt_en() {
    RTC.enableAlarm(ALARM2_ADDRESS);
	cli();
	EICRA |= 0x02; // Falling edge interrupt
	EIMSK |= INT0_EN_MASK; // Set interrupt mask on INT0
	sei();
    RTC.resetAlarms();
}

void clock_intrpt_dis() {
    RTC.disableAlarm(ALARM2_ADDRESS);
	cli();
	EIMSK &= ~(INT0_EN_MASK); // Disable interrupt
	sei();
}

void clock_intrpt_ack() {
    RTC.resetAlarms();
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
