#include "disp.h"
#include <avr/pgmspace.h>
#include <GxEPD2_AVR_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <TimeLib.h>
#include "clock.h"

GxEPD2_AVR_BW display(GxEPD2::GDEH029A1, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
// Fonts
extern const uint8_t u8g2_font_droidserif_108pt[] U8G2_FONT_SECTION("u8g2_font_droidserif_108pt");
extern const uint8_t u8g2_font_timR14[] U8G2_FONT_SECTION("u8g2_font_timR14");

static const char AM_STR[] = "AM";
static const char PM_STR[] = "PM";
static char date_buf[31]; // Longest date string: "Wednesday, September 20, 2019" = 30 chars + \0
static char time_buf[6]; // Longest time "12:00" = 4 chars + \0

void disp_init() {
    display.init();
    display.setRotation(1);
    u8g2Fonts.begin(display);

    // TODO: interrupt?
}

void disp_clear() {
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
    } while(display.nextPage()); 
}

void disp_time() {
    // Date
    sprintf(date_buf, "%s, %s %d, %d", dayStr(weekday()),
            monthStr(month()), day(), year());

    // Time
    sprintf(time_buf, "%2d:%02d", hourFormat12(), minute());

    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        
        // Meridian and Date settings
        u8g2Fonts.setFont(u8g2_font_timR14);
        
        // Draw Meridian
        u8g2Fonts.setCursor(display.width() - 8 - 25, u8g2Fonts.getFontAscent() + 8);
        if (isPM()) {
            display.writeFillRect(display.width() - 16 - 24, 0,
                    16+25, u8g2Fonts.getFontAscent() + 16, GxEPD_BLACK);
            u8g2Fonts.setForegroundColor(GxEPD_WHITE);
            u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
            u8g2Fonts.print(PM_STR);
        }
        else {
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.print(AM_STR);
        }


        // Date and Time Settings 
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        // Draw date string
        u8g2Fonts.setCursor(8, u8g2Fonts.getFontAscent() + 8);
        u8g2Fonts.print(date_buf);
   
        // Draw time string
        u8g2Fonts.setFont(u8g2_font_droidserif_108pt);
        u8g2Fonts.setCursor(8, display.height()-8); // x = 10 is good for 10, 11, 12
        u8g2Fonts.print(time_buf);
    } while (display.nextPage());
}

