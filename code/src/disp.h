#ifndef _DISP_H_
#define _DISP_H_

#include <stdint.h>
#include <TimeLib.h>
#include "op.h"

void disp_init();
void disp_on();
void disp_off();

void disp_clear();
void disp_update(uint8_t refresh=0);

void disp_setDoT(uint8_t dateTime_n);

void disp_setDate(uint16_t yr, int8_t m, int8_t d);
void disp_setDateYear(uint16_t yr, uint8_t sel);
void disp_setDateMonth(int8_t m, uint8_t sel);
void disp_setDateDay(int8_t d, uint8_t sel);

void disp_setTime(op_meridiem_t meridiem, int8_t h, int8_t m);
void disp_setTimeMeridiem(op_meridiem_t meridiem, uint8_t sel);
void disp_setTimeHour(int8_t h, uint8_t sel);
void disp_setTimeMinute(int8_t m, uint8_t sel);

void disp_setSave();
void disp_setSaving();

#endif
