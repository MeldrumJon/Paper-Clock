#include "disp.h"
#include <avr/pgmspace.h>
#include <GxEPD2_AVR_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <TimeLib.h>
#include "clock.h"

// Fonts
#define DATE_FONT_HEIGHT 13
#define MERIDIEM_FONT_HEIGHT 13
#define TIME_FONT_HEIGHT 1
// Display
#define DISP_WIDTH 296
#define DISP_HEIGHT 128
#define MERIDIEM_WIDTH 25
#define MERIDIEM_HEIGHT MERIDIEM_FONT_HEIGHT
#define MERIDIEM_PADDING 6
#define MERIDIEM_CURSOR_X (DISP_WIDTH-(MERIDIEM_WIDTH+MERIDIEM_PADDING))
#define MERIDIEM_CURSOR_Y (DISP_HEIGHT-MERIDIEM_PADDING)
#define MERIDIEM_RECT_WIDTH (MERIDIEM_WIDTH-1+MERIDIEM_PADDING*2)
#define MERIDIEM_RECT_HEIGHT (MERIDIEM_HEIGHT+MERIDIEM_PADDING*2)
#define MERIDIEM_RECT_X (DISP_WIDTH-MERIDIEM_RECT_WIDTH)
#define MERIDIEM_RECT_Y (DISP_HEIGHT-MERIDIEM_RECT_HEIGHT)

#define DATE_Y (DATE_FONT_HEIGHT+15)

#define TIME_AREA_WIDTH (DISP_WIDTH - MERIDIEM_RECT_WIDTH)
#define TIME_Y (DISP_HEIGHT-8)

GxEPD2_AVR_BW display(GxEPD2::GDEH029A1, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
// Fonts
//extern const uint8_t u8g2_font_droidserif_108pt[] U8G2_FONT_SECTION("u8g2_font_droidserif_108pt");
extern const uint8_t u8g2_font_droidserif_96pt[] U8G2_FONT_SECTION("u8g2_font_droidserif_96pt");
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

void disp_time(time_t t) {
    // Date
    sprintf(date_buf, "%s, %s %d, %d", dayStr(weekday(t)),
            monthStr(month(t)), day(t), year(t));

    u8g2Fonts.setFont(u8g2_font_timR14);
    uint16_t dateWidth = u8g2Fonts.getUTF8Width(date_buf);
    uint8_t dateX = (DISP_WIDTH-dateWidth)/2;

    // Time
    sprintf(time_buf, "%d:%02d", hourFormat12(t), minute(t));

    u8g2Fonts.setFont(u8g2_font_droidserif_96pt);
    uint16_t timeWidth = u8g2Fonts.getUTF8Width(time_buf);
    uint8_t timeX = (TIME_AREA_WIDTH-timeWidth)/2;
    if (hourFormat12(t) >= 10) { timeX -= 2; }

    uint8_t pm = isPM(t);

    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);

        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        // Draw date string
        u8g2Fonts.setFont(u8g2_font_timR14);
        u8g2Fonts.setCursor(dateX, DATE_Y);
        u8g2Fonts.print(date_buf);
   
        // Draw time string
        u8g2Fonts.setFont(u8g2_font_droidserif_96pt);
        u8g2Fonts.setCursor(timeX, TIME_Y); // x = 10 is good for 10, 11, 12
        u8g2Fonts.print(time_buf);
        
        // Draw Meridian
        u8g2Fonts.setFont(u8g2_font_timR14);
        u8g2Fonts.setCursor(MERIDIEM_CURSOR_X, MERIDIEM_CURSOR_Y);
        if (pm) {
            display.writeFillRect(MERIDIEM_RECT_X, MERIDIEM_RECT_Y,
                    MERIDIEM_RECT_WIDTH, MERIDIEM_RECT_HEIGHT,
                    GxEPD_BLACK);
            u8g2Fonts.setForegroundColor(GxEPD_WHITE);
            u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
            u8g2Fonts.print(PM_STR);
        }
        else {
            display.writeFastHLine(MERIDIEM_RECT_X, MERIDIEM_RECT_Y, MERIDIEM_RECT_WIDTH, GxEPD_BLACK);
            display.writeFastVLine(MERIDIEM_RECT_X, MERIDIEM_RECT_Y, MERIDIEM_RECT_HEIGHT, GxEPD_BLACK);
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.print(AM_STR);
        }
    } while (display.nextPage());
}

