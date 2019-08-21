// mapping 
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11


#include <Arduino.h>
#include <TimeLib.h>
#include "usart.h"
#include "clock.h"
#include "disp.h"

static time_t t;

void setup()
{
    // Serial

    usart_init();
 	stdout = &uart_stream;
 	printf("Hello world!\r\n");

    // Clock
    
    clock_init();
    //clock_set(19, 8, 20, 17, 52, 00);
    t = clock_read();


    // Display

    disp_init();
    disp_clear();
}

void loop()
{
    time_t t = clock_read();
    disp_update(t);
    _delay_ms(5000);
}
