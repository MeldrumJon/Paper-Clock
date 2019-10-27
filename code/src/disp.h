#ifndef _DISP_H_
#define _DISP_H_

#include <stdint.h>
#include <TimeLib.h>
#include "clock.h"
#include "setting.h"

#define SET_DATE !(0)
#define SET_TIME 0

typedef enum date_enum {
    DRAW_YEAR,
    DRAW_MONTH,
    DRAW_DAY,
    DRAW_NONE
} drawDate_t;

void disp_init();
void disp_on();
void disp_off();

void disp_clear();
void disp_update(time_t t, uint8_t refresh=0);


void disp_dateOrTime(uint8_t dateTime_n, uint8_t firstRun);
void disp_setDate(uint16_t yr, uint8_t m, uint8_t d, drawDate_t select_ymdn, 
        uint8_t update, uint8_t firstRun);

void disp_setting(setting_t setting);
void disp_showSet(uint16_t num);
void disp_showMeridiem();
void disp_confirm(uint8_t isSetting);

#endif
