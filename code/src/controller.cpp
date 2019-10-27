// #define CONTROLLER_DEBUG

#include "controller.h"
#include <util/delay.h>
#ifdef CONTROLLER_DEBUG
#include <stdio.h>
#endif
#include "clock.h"
#include "disp.h"
#include "btn.h"
#include "timer.h"
#include "encoder.h"
#include "setting.h"

static enum controller_st_t {
	INIT_ST,
	OFF_ST,
	WAKE_UP_ST,
    WHICH_ST,
	SET_MERIDIEM_ST,
	SET_HOUR_ST,
	SET_MINUTE_ST,
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
    time_t t = clock_read();
    disp_update(t, 1);
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
			case SET_MERIDIEM_ST:
				printf("SET_MERIDIEM_ST\r\n");
				break;
			case SET_HOUR_ST:
				printf("SET_HOUR_ST\r\n");
				break;
			case SET_MINUTE_ST:
				printf("SET_MINUTE_ST\r\n");
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
    static int16_t y;
    static int8_t mo;
    static int8_t d;
    static int8_t h;
    static int8_t m;
    static int8_t pm;
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
		case SET_MERIDIEM_ST:
            ++timeout_cnt;
			break;
		case SET_HOUR_ST:
            ++timeout_cnt;
			break;
		case SET_MINUTE_ST:
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
                d_o_t = 0;
                time_t t = clock_read();
                y = year(t);
                mo = month(t);
                d = day(t);
                h = (setting_getMeridiem() != MILITARY) ? hourFormat12(t) : hour(t);
                m = minute(t);
                pm = isPM(t);

				timeout_cnt = 0;
                disp_dateOrTime(d_o_t, 1);
                currentState = WHICH_ST;
			}
			else if (timeout_cnt >= WAKE_TIMEOUT) {
				currentState = OFF_ST;
			}
			break;
        case WHICH_ST:
            if (delta & 0x1) { // Odd number of turns
                d_o_t = (!d_o_t); // Toggle date or time
                disp_dateOrTime(d_o_t, 0);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                if (d_o_t) {
                    disp_setDate(y, mo, d, DRAW_YEAR, 0, 1);
                    currentState = DATE_SET_YEAR_ST;
                }
                else {
                    // TODO: show next setting
                    // TODO: move to next state
                }
            }
            else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
                currentState = OFF_ST;
            }
            break;
		case DATE_SET_YEAR_ST:
            if (delta) {
                y += delta;
                if (y < 2000) { y = 2000; }
                if (y > 2100) { y = 2100; }
                disp_setDate(y, mo, d, DRAW_YEAR, 1, 0);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setDate(y, mo, d, DRAW_MONTH, 0, 0);
                currentState = DATE_SET_MONTH_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case DATE_SET_MONTH_ST:
            if (delta) {
                mo += delta;
                if (mo < 1) { mo = 1; }
                if (mo > 12) { mo = 12; }
                disp_setDate(y, mo, d, DRAW_MONTH, 1, 0);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setDate(y, mo, d, DRAW_DAY, 0, 0);
                currentState = DATE_SET_DAY_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case DATE_SET_DAY_ST:
            if (delta) {
                d += delta;
                if (d > 31) { d = 31; }
                if (d < 1) { d = 1; }
                disp_setDate(y, mo, d, DRAW_DAY, 1, 0);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setDate(y, mo, d, DRAW_NONE, 0, 0);
                disp_confirm(0);
                currentState = CONFIRM_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case SET_MERIDIEM_ST:
            if (delta) {
                while (delta>0) {
                    setting_meridiemNext(0);
                    delta--;
                }
                while (delta<0) {
                    setting_meridiemNext(1);
                    delta++;
                }
                disp_showMeridiem();
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setting(HOUR);
                disp_showSet(h);
                currentState = SET_HOUR_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case SET_HOUR_ST:
            if (delta) {
                h += delta;
                if (setting_getMeridiem() != MILITARY) {
                    if (h > 12) { h = 12; }
                    if (h < 1) { h = 1; }
                }
                else {
                    if (h > 23) { h = 23; }
                    if (h < 0) { h = 0; }
                }
                disp_showSet(h);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_setting(MINUTE);
                disp_showSet(m);
                currentState = SET_MINUTE_ST;
            }
			else if (timeout_cnt >= INPUT_TIMEOUT) {
                powerdown();
				currentState = OFF_ST;
			}
			break;
		case SET_MINUTE_ST:
            if (delta) {
                m += delta;
                if (m > 59) { m = 59; }
                if (m < 0) { m = 0; }
                disp_showSet(m);
                timeout_cnt = 0;
            }
            else if (btn_press_flag) {
                timeout_cnt = 0;
                disp_confirm(0);
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
                if (d_o_t) { // Date
                    
                }
                else {
                }
                disp_confirm(1);
                uint8_t y2k_year = y - 2000;
                setting_meridiem_t meridiem = setting_getMeridiem();
                if (meridiem != MILITARY) {
                    h = clock_12to24(h, (meridiem==PM));
                }
                clock_set(y2k_year, mo, d, h, m, 0);
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
