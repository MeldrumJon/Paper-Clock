#ifndef _DISP_H_
#define _DISP_H_

#include <stdint.h>
#include <TimeLib.h>
#include "clock.h"

void disp_init();
void disp_on();
void disp_off();

void disp_update(time_t t, uint8_t refresh=0);

#endif
