// mapping 
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

#include <avr/power.h>
#include <avr/sleep.h>
#include <Arduino.h>
#include <TimeLib.h>
#include "usart.h"
#include "clock.h"
#include "disp.h"

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
    power_usart0_enable();
    usart_init();
 	stdout = &uart_stream;
 	printf("Hello world!\r\n");

    // Clock
    
    clock_init();
    clock_intrpt_en();
    //clock_set(19, 8, 20, 17, 52, 00);

    // Display
    disp_init();

    time_t t = clock_read();
    disp_update(t);
    disp_off();
}

void loop()
{
    cli();
    if (clk_intrpt_flag) {
        clk_intrpt_flag = 0;
        sei();
        clock_intrpt_ack();

        printf("Clock interrupt!\r\n");

        time_t t = clock_read();
        disp_on();
        disp_update(t);
        disp_off();
    }
    sei();

    cli();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
  	sei();
  	sleep_cpu();
  	sleep_disable();
}

