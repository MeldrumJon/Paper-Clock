// #define CONTROLLER_DEBUG

#include "controller.h"
#include <util/delay.h>
#ifdef CONTROLLER_DEBUG
#include <stdio.h>
#endif
#include <TimeLib.h>
#include "clock.h"
#include "disp.h"
#include "btn.h"
#include "timer.h"
#include "encoder.h"
#include "op.h"

static enum controller_st_t {
	INIT_ST,
	OFF_ST,
	WAKE_UP_ST,
    WHICH_ST,
	TIME_SET_MERIDIEM_ST,
	TIME_SET_HOUR_ST,
	TIME_SET_MINUTE_ST,
	DATE_SET_YEAR_ST,
	DATE_SET_MONTH_ST,
	DATE_SET_DAY_ST,
    CONFIRM_ST
} currentState = INIT_ST;

#define WAKE_TIMEOUT TIMER_TICKS(250)
#define INPUT_TIMEOUT TIMER_TICKS(30000UL)

static uint16_t timeout_cnt;

// Save power by disabling peripherals.
static void powerdown(void) {
    tmElements_t* tm = clock_read();
    op_setTime(tm);
    disp_update(1);
	encoder_disable(); // Don't disable buttons, we need them to wake up!
}

static void powerup(void) {
	encoder_enable();
}

// static uint16_t mins;
// static uint16_t ui_to_cnt;

#ifdef CONTROLLER_DEBUG
// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint(void) {
	static controller_st_t previousState;
	static bool firstPass = true;
	// Only print the message if:
	// 1. This the first pass and the value for previousState is unknown.
	// 2. previousState != currentState - this prevents reprinting the same state name over and over.
	if (previousState != currentState || firstPass) {
    	firstPass = false;                // previousState will be defined, firstPass is false.
    	previousState = currentState;     // keep track of the last state that you were in.
		switch (currentState) {
			case INIT_ST:
				printf("INIT_ST\r\n");
				break;
			case OFF_ST:
				printf("OFF_ST\r\n");
				break;
			case WAKE_UP_ST:
				printf("WAKE_UP_ST\r\n");
				break;
            case WHICH_ST:
                printf("WHICH_ST\r\n");
                break;
			case TIME_SET_MERIDIEM_ST:
				printf("TIME_SET_MERIDIEM_ST\r\n");
				break;
			case TIME_SET_HOUR_ST:
				printf("TIME_SET_HOUR_ST\r\n");
				break;
			case TIME_SET_MINUTE_ST:
				printf("TIME_SET_MINUTE_ST\r\n");
				break;
			case DATE_SET_YEAR_ST:
				printf("DATE_SET_YEAR_ST\r\n");
				break;
			case DATE_SET_MONTH_ST:
				printf("DATE_SET_MONTH_ST\r\n");
				break;
			case DATE_SET_DAY_ST:
				printf("DATE_SET_DAY_ST\r\n");
				break;
    		case CONFIRM_ST:
				printf("CONFIRM_ST\r\n");
				break;
		}
	}
}
#else
void debugStatePrint(void) {}
#endif

void controller_wakeup(void) {
	if (currentState == OFF_ST) {
		timeout_cnt = 0;
		currentState = WAKE_UP_ST;
	}
	
	debugStatePrint();
	return;
}

uint_fast8_t controller_isOff(void) {
	return currentState == OFF_ST;
}

void controller_tick(void) {
    static uint8_t d_o_t;
	int_fast8_t delta = encoder_delta();

	// State action
	switch (currentState) {
		case INIT_ST:
			break;
		case OFF_ST:
            // No ticks will happen while in this state.
			break;
		case WAKE_UP_ST:
            ++timeout_cnt;
			break;
        case WHICH_ST:
            ++timeout_cnt;
            break;
		case TIME_SET_MERIDIEM_ST:
            ++timeout_cnt;
			break;
		case TIME_SET_HOUR_ST:
            ++timeout_cnt;
			break;
		case TIME_SET_MINUTE_ST:
            ++timeout_cnt;
			break;
		case DATE_SET_YEAR_ST:
            ++timeout_cnt;
			break;
		case DATE_SET_MONTH_ST:
            ++timeout_cnt;
			break;
		case DATE_SET_DAY_ST:
            ++timeout_cnt;
			break;
		case CONFIRM_ST:
            ++timeout_cnt;
			break;
		default:
			currentState = INIT_ST;
			break;
	}
	
	// State transition
	switch (currentState) {
		case INIT_ST:
			currentState = OFF_ST;
			break;
		case OFF_ST:
			// Transition happens in controller_wakeup() function
			break;
		case WAKE_UP_ST:
			if (btn_press_flag) {
				powerup();
                d_o_t = 1;
                tmElements_t* tm = clock_read();
                op_setTime(tm);

				timeout_cnt = 0;
                disp_clear();
                disp_setDoT(d_o_t);
                currentState = WHICH_ST;
			}
			else if (timeout_cnt >= WAKE_TIMEOUT) {
				currentState = OFF_ST;
			}
			break;
        case WHICH_ST:
            if (delta & 0x1) { // Odd number of turns
                d_o_t = (!d_o_t); // Toggle date or time
                disp_setDoT(d_o_t);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                if (d_o_t) {
                    disp_setDate(op_year, op_month, op_day);
                    disp_setDateYear(op_year, 1);
                    currentState = DATE_SET_YEAR_ST;
                }
                else {
                    disp_setTime(op_meridiem, op_hour, op_minute);
                    disp_setTimeMeridiem(op_meridiem, 1);
                    currentState = TIME_SET_MERIDIEM_ST;
                }
            }
            else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
                currentState = OFF_ST;
            }
            break;
		case DATE_SET_YEAR_ST:
            if (delta) {
                op_changeYear(delta);
                disp_setDateYear(op_year, 1);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setDateYear(op_year, 0);
                disp_setDateMonth(op_month, 1);
                currentState = DATE_SET_MONTH_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case DATE_SET_MONTH_ST:
            if (delta) {
                op_changeMonth(delta);
                disp_setDateMonth(op_month, 1);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setDateMonth(op_month, 0);
                disp_setDateDay(op_day, 1);
                currentState = DATE_SET_DAY_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case DATE_SET_DAY_ST:
            if (delta) {
                op_changeDay(delta);
                disp_setDateDay(op_day, 1);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setDateDay(op_day, 0);
                disp_setSave();
                currentState = CONFIRM_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case TIME_SET_MERIDIEM_ST:
            if (delta) {
                op_changeMeridiem(delta);
                disp_setTimeMeridiem(op_meridiem, 1);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setTimeMeridiem(op_meridiem, 0);
                if (op_meridiem != MIL) { op_hour = clock_24to12(op_hour); }
                disp_setTimeHour(op_hour, 1);
                currentState = TIME_SET_HOUR_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case TIME_SET_HOUR_ST:
            if (delta) {
                op_changeHour(delta);
                disp_setTimeHour(op_hour, 1);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setTimeHour(op_hour, 0);
                disp_setTimeMinute(op_minute, 1);
                currentState = TIME_SET_MINUTE_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case TIME_SET_MINUTE_ST:
            if (delta) {
                op_changeMinute(delta);
                disp_setTimeMinute(op_minute, 1);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setTimeMinute(op_minute, 0);
                disp_setSave();
                currentState = CONFIRM_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case CONFIRM_ST:
            if (delta) {
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                disp_setSaving();
                tmElements_t* tm = clock_read();
                if (d_o_t) { // Date
                    clock_set(op_year, op_month, op_day,
                            tm->Hour, tm->Minute, tm->Second);
                }
                else {
                    int8_t hour = (op_meridiem == MIL) ? op_hour
                            : clock_12to24(op_hour, (op_meridiem == PM));
                    clock_set(tmYearToCalendar((uint16_t)tm->Year), tm->Month, tm->Day, 
                            hour, op_minute, 0);
                }
                powerdown();
                currentState = OFF_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}            
			break;
		default:
			currentState = INIT_ST;
			break;
	}
	
	debugStatePrint();
	return;
}
