/* Would ( (i % n) + n) % n be better for circular wraparound? MATH_MOD */

#include "op.h"
#include "clock.h"

#define IS_LEAP_YEAR(y) (!(y % 4) && y % 100 || !(y % 400))

#define MATH_MOD(x,n) ((((x)%(n))+(n))%(n))

op_meridiem_t op_meridiem = MIL;
uint16_t op_year = 2000;
int8_t op_month = 1;
int8_t op_day = 1;
int8_t op_hour = 0;
int8_t op_minute = 0;

void op_setTime(tmElements_t* tm) {
    // Date
    op_year = tmYearToCalendar(tm->Year);
    op_month = tm->Month;
    op_day = tm->Day;

    // TIME
    if (op_meridiem == MIL) {
        op_hour = tm->Hour;
    }
    else {
        op_meridiem = (tm->Hour >= 12) ? PM : AM;
        op_hour = clock_24to12(tm->Hour);
    }
    op_minute = tm->Minute;
    return;
}

void op_changeYear(int_fast8_t delta) {
    int16_t diff = op_year - OP_MIN_YEAR; // 2000-2100 becomes 0-100
    diff += delta;
    diff = MATH_MOD(diff, OP_YEAR_RANGE+1);
    op_year = diff + OP_MIN_YEAR; // Back to 2000-2100
    return;
}

void op_changeMonth(int_fast8_t delta) {
    op_month -= 1; // 1-12 becomes 0-11
    op_month += delta;
    op_month = MATH_MOD(op_month, 12);
    op_month += 1; // Back to 1-12.
    return;
}

void op_changeDay(int_fast8_t delta) {
    uint8_t month_days;
    switch (op_month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
        default:
            month_days = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            month_days = 30;
            break;
        case 2:
            month_days = IS_LEAP_YEAR(op_year) ? 29 : 28;
    }
    op_day -= 1; // 1-month_days become 0-month_days-1
    op_day += delta;
    op_day = MATH_MOD(op_day, month_days);
    op_day += 1;
    return;
}

void op_changeMeridiem(int_fast8_t delta) {
    int num = (int) op_meridiem;
    num += delta;
    num = MATH_MOD(num, OP_NUM_MERIDIEM_OPTS);
    op_meridiem = (op_meridiem_t) num;
    return;
}

void op_changeHour(int_fast8_t delta) {
    op_hour += delta;
    if (op_meridiem != MIL) {
        op_hour -= 1; // 1-12 becomes 0-11
        op_hour = MATH_MOD(op_hour, 12);
        op_hour += 1; // Back to 1-12.
    }
    else {
        op_hour = MATH_MOD(op_hour, 24);
    }
    return;
}

void op_changeMinute(int_fast8_t delta) {
    op_minute += delta;
    op_minute = MATH_MOD(op_minute, 60);
    return;
}

