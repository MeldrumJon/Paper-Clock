#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_PRESCALER 1024UL
#define TIMER_OC 155UL
#define TIMER_TICKS(ms) ((ms*((F_CPU/1000UL))/(TIMER_PRESCALER*(1UL+TIMER_OC))))

void timer_enable(void);
void timer_disable(void);

void timer_wait(uint16_t ticks);

extern volatile uint_fast8_t timer_intrpt_flag;

#endif
