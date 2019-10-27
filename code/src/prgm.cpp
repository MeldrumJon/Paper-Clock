// mapping 
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

#include <avr/power.h>
#include <avr/sleep.h>
#include <Arduino.h>
#include <TimeLib.h>
//#include "usart.h"
#include "btn.h"
#include "encoder.h"
#include "timer.h"
#include "btn.h"
#include "clock.h"
#include "disp.h"
#include "controller.h"

void setup()
{
    // Power saving
	DDRD = 0x00; // Set pins as input
	DDRC = 0x00;
	DDRB = 0x00;
	PORTD = 0x00;
	PORTC = 0x00;
	PORTB = 0x00;
	ADCSRA = 0; // disable ADC
	power_all_disable();
    power_twi_enable();
	power_spi_enable();
	power_timer2_enable();
    //power_timer0_enable(); // delay()

    // Serial
    //power_usart0_enable();
    //usart_init();
 	//stdout = &uart_stream;
 	//printf("Hello world!\r\n");

    // IO
    btn_enable();
    encoder_enable();

    // Timer
    timer_enable();

    // Clock
    
    clock_init();
    clock_intrpt_en();

    // Display
    disp_init();
    disp_clear();

    time_t t = clock_read();
    disp_update(t);
}

void loop()
{
    cli();
	if (btn_intrpt_flag) {
		btn_intrpt_flag = 0;
		sei();

		controller_wakeup();
	}
	sei();

	cli();
	if (timer_intrpt_flag) {
		timer_intrpt_flag = 0;
		sei();
		
		btn_tick();
		controller_tick();
	}
	sei();

    cli();
    if (clk_intrpt_flag) {
        clk_intrpt_flag = 0;
        sei();
        clock_intrpt_ack();

        if (controller_isOff()) {
            time_t t = clock_read();
            disp_update(t);
        }
    }
    sei();

    cli();
	if (controller_isOff()) {
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	}
	else {
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	}
    sleep_enable();
  	sei();
  	sleep_cpu();
  	sleep_disable();
}

