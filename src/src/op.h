#ifndef OP_H
#define OP_H

#include <stdint.h>
#include <TimeLib.h>

#define OP_MIN_YEAR 2000
#define OP_MAX_YEAR 2100
#define OP_YEAR_RANGE (OP_MAX_YEAR-OP_MIN_YEAR)

#define OP_NUM_MERIDIEM_OPTS 3

typedef enum op_meridiem_enum {
    MIL=0,
    AM=1,
    PM=2
} op_meridiem_t;

extern op_meridiem_t op_meridiem;
extern uint16_t op_year;
extern int8_t op_month;
extern int8_t op_day;
extern int8_t op_hour;
extern int8_t op_minute;

void op_setTime(tmElements_t* tm);

void op_changeYear(int_fast8_t delta);
void op_changeMonth(int_fast8_t delta);
void op_changeDay(int_fast8_t delta);
void op_changeMeridiem(int_fast8_t delta);
void op_changeHour(int_fast8_t delta);
void op_changeMinute(int_fast8_t delta);

#endif
