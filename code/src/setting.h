#ifndef SETTING_H
#define SETTING_H

#include <stdint.h>

// All but MERIDIEM handled by RTC
typedef enum setting_enum {
    YEAR,
    MONTH,
    DAY,
    MERIDIEM,
    HOUR,
    MINUTE
} setting_t;

#define SETTING_NUM_MERIDIEM 3

typedef enum setting_meridiem_enum {
    AM,
    PM,
    MILITARY
} setting_meridiem_t;

void setting_setMeridiem(setting_meridiem_t meridiem);
setting_meridiem_t setting_getMeridiem();
void setting_meridiemNext(uint8_t backwards);

#endif
