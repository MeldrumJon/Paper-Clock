// mapping 
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11

#include <Arduino.h>
#include <GxEPD2_AVR_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "usart.h"

extern const uint8_t u8g2_font_times_118pt[] U8G2_FONT_SECTION("u8g2_font_times_118pt");

GxEPD2_AVR_BW display(GxEPD2::GDEH029A1, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void setup()
{
 	usart_init();
 	stdout = &uart_stream;
 	printf("Hello world!\r\n");

    // Use setPartialWindow() to do partial update.  See basic example
    display.init();
    display.setRotation(1);
    display.setFullWindow();
    u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX

    u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
    u8g2Fonts.setFontDirection(0);            // left to right (this is default)
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);         // apply Adafruit GFX color
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);         // apply Adafruit GFX color
    u8g2Fonts.setFont(u8g2_font_times_118pt);  // select u8g2 

//   display.firstPage();
//   do
//   {
//       display.fillScreen(GxEPD_WHITE);
//   }
//   while (display.nextPage());
}

#define TIME_HEIGHT 81
#define TIME_WIDTH 267

static uint8_t hrs = 0;
static uint8_t mins = 0;
static char time_str[] = "12:00"; // Height: 79px, Width: 266px
void loop()
{
    sprintf(time_str, "%2.2d:%2.2d", hrs+1, mins);

    display.setPartialWindow(14, 
            46, 
            TIME_WIDTH, 
            TIME_HEIGHT);

    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        u8g2Fonts.setCursor(15, display.height()-1);
        u8g2Fonts.print(time_str);
    }
    while (display.nextPage());

    hrs = (hrs + 1) % 12;
    mins = (mins + 1) % 60;

}
