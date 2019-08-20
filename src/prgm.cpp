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
    clock_set(19, 9, 25, 00, 45, 00);


    // Display

    disp_init();
    disp_clear();

    time_t t = clock_read();

    printf("%d-%d-%d %d:%02d\r\n", year(t), month(t), day(t), hour(t), 
            minute(t));
    
    disp_time(t);
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
