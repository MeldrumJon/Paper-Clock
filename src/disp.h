#ifndef _DISP_H_
#define _DISP_H_

#include <stdint.h>
#include <TimeLib.h>
#include "clock.h"
#include "setting.h"

void disp_init();
void disp_on();
void disp_off();

void disp_clear();
void disp_update(time_t t, uint8_t refresh=0);

void disp_setting(setting_t setting);
void disp_showSet(uint16_t num);
void disp_showMeridiem();
void disp_confirm(uint8_t isSetting);

#endif
