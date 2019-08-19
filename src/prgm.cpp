// mapping 
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11


#include <Arduino.h>
#include <TimeLib.h>
#include "usart.h"
#include "clock.h"
#include "disp.h"

void setup()
{
    // Serial

    usart_init();
 	stdout = &uart_stream;
 	printf("Hello world!\r\n");

    // Clock
    
    clock_init();
    clock_set(19, 9, 27, 18, 45, 00);


    // Display

    disp_init();
    disp_clear();

    setTime(clock_read()); // Use timeLib to set current time.

    printf("%d-%d-%d %d:%02d", year(), month(), day(), hour(), minute());
    
    disp_time();
}

void loop()
{
    //time_t time;
    //clock_read(&time);

    //char buf[20];
    //format_time_str(&time, buf);
    //printf("%s\r\n", buf);

    //disp_partial_time(time.hour, time.minute);
}
