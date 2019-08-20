#ifndef _DISP_H_
#define _DISP_H_

#include <stdint.h>
#include "clock.h"

void disp_init();
void disp_clear();

void disp_time(time_t t);

#endif
