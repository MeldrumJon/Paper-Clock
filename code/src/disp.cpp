#include "disp.h"
#include <avr/pgmspace.h>
#define DISABLE_DIAGNOSTIC_OUTPUT
#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <TimeLib.h>
#include "clock.h"
#include "op.h"

// Fonts
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
extern const uint8_t u8g2_font_droidserif_96pt[] U8G2_FONT_SECTION("u8g2_font_droidserif_96pt");
extern const uint8_t u8g2_font_timR14_v2[] U8G2_FONT_SECTION("u8g2_font_timR14_v2");

#define DISP_WIDTH 296
#define DISP_HEIGHT 128

#define SMALL_FONT u8g2_font_timR14_v2
#define BIG_FONT u8g2_font_droidserif_96pt

#define SMALL_FONT_ASCENT 13
#define SMALL_FONT_DESCENT 4
#define BIG_FONT_ASCENT 69
#define BIG_FONT_DESCENT 23

// Time Display

#define MERIDIEM_WIDTH 25+1
#define MERIDIEM_HEIGHT SMALL_FONT_ASCENT+1
#define MERIDIEM_PADDING 7
#define MERIDIEM_CURSOR_X (DISP_WIDTH - (MERIDIEM_WIDTH + MERIDIEM_PADDING))
#define MERIDIEM_CURSOR_Y (DISP_HEIGHT - MERIDIEM_PADDING)
#define MERIDIEM_AREA_WIDTH (MERIDIEM_WIDTH + MERIDIEM_PADDING*2 - 2)
#define MERIDIEM_AREA_HEIGHT (MERIDIEM_HEIGHT + MERIDIEM_PADDING*2)
#define MERIDIEM_AREA_X (DISP_WIDTH - MERIDIEM_AREA_WIDTH)
#define MERIDIEM_AREA_Y (DISP_HEIGHT - MERIDIEM_AREA_HEIGHT)

#define TIME_DATE_PADDING 20
#define TIME_DATE_HEIGHT (BIG_FONT_ASCENT + SMALL_FONT_ASCENT + TIME_DATE_PADDING)

#define TIME_CURSOR_Y ((DISP_HEIGHT + TIME_DATE_HEIGHT)/2-1)
#define TIME12_AREA_WIDTH (DISP_WIDTH - (MERIDIEM_AREA_WIDTH+1))
#define TIME24_AREA_WIDTH (DISP_WIDTH)
#define TIME_AREA_HEIGHT ((DISP_HEIGHT - TIME_CURSOR_Y) + BIG_FONT_ASCENT + 2)
#define TIME_AREA_X 0
#define TIME_AREA_Y (TIME_CURSOR_Y - BIG_FONT_ASCENT - 1)

#define DATE_CURSOR_Y (TIME_CURSOR_Y - BIG_FONT_ASCENT - TIME_DATE_PADDING+1)

// Settings
#define SMALL_DATE_WIDTH 36
#define SMALL_TIME_WIDTH 39

#define SET_LINE_SPACING 20

// title
#define SET_TITLE_CURSOR_Y (SET_CURSOR_Y-SET_LINE_SPACING-SMALL_FONT_ASCENT)
#define SET_DATE_TITLE_CURSOR_X ((DISP_WIDTH-SMALL_DATE_WIDTH)/2)
#define SET_TIME_TITLE_CURSOR_X ((DISP_WIDTH-SMALL_TIME_WIDTH)/2)

// "set"
#define SET_SAVE_CURSOR_Y (SET_CURSOR_Y+SET_LINE_SPACING+SMALL_FONT_ASCENT)
#define SET_SAVE_AREA_Y (SET_SAVE_CURSOR_Y-SMALL_FONT_ASCENT-1)
#define SET_SAVE_AREA_HEIGHT (SMALL_FONT_ASCENT+2)

#define SET_SAVE_WIDTH 25
#define SET_SAVING_WIDTH 11
#define SET_SAVE_CURSOR_X ((DISP_WIDTH-SET_SAVE_WIDTH)/2)
#define SET_SAVING_CURSOR_X ((DISP_WIDTH-SET_SAVING_WIDTH)/2)

// actual settings
#define SET_CURSOR_Y ((DISP_HEIGHT+SMALL_FONT_ASCENT)/2)
#define SET_AREA_Y ((DISP_HEIGHT-SMALL_FONT_ASCENT)/2-1)
#define SET_AREA_HEIGHT (SMALL_FONT_ASCENT+2)

// date or time
#define SET_DOT_SPACING 56
#define SET_DOT_WIDTH (SMALL_DATE_WIDTH+SET_DOT_SPACING+SMALL_TIME_WIDTH)

#define SET_DOT_DATE_CURSOR_X ((DISP_WIDTH-SET_DOT_WIDTH)/2)
#define SET_DOT_TIME_CURSOR_X (SET_DOT_DATE_CURSOR_X+SMALL_DATE_WIDTH+SET_DOT_SPACING)

// date
#define SMALL_DASH_WIDTH 5
#define SMALL_YEAR_WIDTH 36
#define SMALL_MONTH_WIDTH 18
#define SMALL_DAY_WIDTH 18

#define SET_DATE_SPACING 16
#define SET_DATE_WIDTH (SMALL_YEAR_WIDTH+SMALL_MONTH_WIDTH+SMALL_DAY_WIDTH+2*SMALL_DASH_WIDTH+4*SET_DATE_SPACING)
#define SET_DATE_CURSOR_X ((DISP_WIDTH-SET_DATE_WIDTH)/2)

#define SET_DATE_YEAR_CURSOR_X (SET_DATE_CURSOR_X)
#define SET_DATE_DASH1_CURSOR_X (SET_DATE_YEAR_CURSOR_X+SMALL_YEAR_WIDTH+SET_DATE_SPACING)
#define SET_DATE_MONTH_CURSOR_X (SET_DATE_DASH1_CURSOR_X+SMALL_DASH_WIDTH+SET_DATE_SPACING)
#define SET_DATE_DASH2_CURSOR_X (SET_DATE_MONTH_CURSOR_X+SMALL_MONTH_WIDTH+SET_DATE_SPACING)
#define SET_DATE_DAY_CURSOR_X (SET_DATE_DASH2_CURSOR_X+SMALL_DASH_WIDTH+SET_DATE_SPACING)

// time
#define SMALL_MERIDIEM_WIDTH 29
#define SMALL_HOUR_WIDTH 18
#define SMALL_COLON_WIDTH 3
#define SMALL_MINUTE_WIDTH 18

#define SET_TIME_MERIDIEM_SPACING 34
#define SET_TIME_COLON_SPACING 12

#define SET_TIME_WIDTH (SMALL_MERIDIEM_WIDTH+SMALL_HOUR_WIDTH+SMALL_MINUTE_WIDTH+SMALL_COLON_WIDTH+SET_TIME_MERIDIEM_SPACING+2*SET_TIME_COLON_SPACING)
#define SET_TIME_CURSOR_X ((DISP_WIDTH-SET_TIME_WIDTH)/2)

#define SET_TIME_MERIDIEM_CURSOR_X SET_TIME_CURSOR_X
#define SET_TIME_HOUR_CURSOR_X (SET_TIME_MERIDIEM_CURSOR_X+SMALL_MERIDIEM_WIDTH+SET_TIME_MERIDIEM_SPACING)
#define SET_TIME_COLON_CURSOR_X (SET_TIME_HOUR_CURSOR_X+SMALL_HOUR_WIDTH+SET_TIME_COLON_SPACING)
#define SET_TIME_MINUTE_CURSOR_X (SET_TIME_COLON_CURSOR_X+SMALL_COLON_WIDTH+SET_TIME_COLON_SPACING)

// Display
#define MAX_DISPAY_BUFFER_SIZE 800 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_290, MAX_HEIGHT(GxEPD2_290)> display(GxEPD2_290(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));


// Strings
static const char MIL_STR[] = " 24";
static const char AM_STR[] = "AM";
static const char PM_STR[] = "PM";
static const char* MERIDIEM_STR[3] = {
    MIL_STR,
    AM_STR,
    PM_STR
};
static const char DATE_STR[] = "Date";
static const char TIME_STR[] = "Time";
static const char SAVE_STR[] = "Set";
static const char SAVING_STR[] = "...";

// Variables

// Date/Time Buffers
static char date_buf[31]; // Longest date string: "Wednesday, September 20, 2019" = 30 chars + \0
static char time_buf[6]; // Longest time "12:00" = 4 chars + \0

// Buffers for settings
static char set_buf1[5];
static char set_buf2[3];
static char set_buf3[3];

static uint16_t dateWidth;
static uint16_t timeWidth;
static uint8_t dateX;
static uint8_t timeX;

static enum updateMode_t {
    FULL,
    TIME
} updateMode;

static void _drawFull() {
    display.setFullWindow();
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);

        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

        // Draw date string
        u8g2Fonts.setFont(SMALL_FONT);
        u8g2Fonts.setCursor(dateX, DATE_CURSOR_Y);
        u8g2Fonts.print(date_buf);
   
        // Draw time string
        u8g2Fonts.setFont(BIG_FONT);
        u8g2Fonts.setCursor(timeX, TIME_CURSOR_Y);
        u8g2Fonts.print(time_buf);
        
        // Draw Meridian
        if (op_meridiem != MIL) {
            u8g2Fonts.setFont(SMALL_FONT);
            u8g2Fonts.setCursor(MERIDIEM_CURSOR_X, MERIDIEM_CURSOR_Y);
            if (op_meridiem == PM) {
                display.writeFillRect(MERIDIEM_AREA_X, MERIDIEM_AREA_Y,
                        MERIDIEM_AREA_WIDTH, MERIDIEM_AREA_HEIGHT,
                        GxEPD_BLACK);
                u8g2Fonts.setForegroundColor(GxEPD_WHITE);
                u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
                u8g2Fonts.print(MERIDIEM_STR[(int)op_meridiem]);
            }
            else {
                display.writeFastHLine(MERIDIEM_AREA_X, MERIDIEM_AREA_Y, MERIDIEM_AREA_WIDTH, GxEPD_BLACK);
                display.writeFastVLine(MERIDIEM_AREA_X, MERIDIEM_AREA_Y, MERIDIEM_AREA_HEIGHT, GxEPD_BLACK);
                u8g2Fonts.setForegroundColor(GxEPD_BLACK);
                u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
                u8g2Fonts.print(MERIDIEM_STR[(int)op_meridiem]);
            }
        }
    } while (display.nextPage());
    display.hibernate();
    return;
}

static void _drawTime() {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setFont(BIG_FONT);
    
    if (op_meridiem == MIL) {
        display.setPartialWindow(TIME_AREA_X, TIME_AREA_Y,
                TIME24_AREA_WIDTH, TIME_AREA_HEIGHT);
    }
    else {
        display.setPartialWindow(TIME_AREA_X, TIME_AREA_Y,
                TIME12_AREA_WIDTH, TIME_AREA_HEIGHT);
    }
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        // Draw Time String
        u8g2Fonts.setCursor(timeX, TIME_CURSOR_Y);
        u8g2Fonts.print(time_buf);
    } while (display.nextPage());
    display.hibernate();
    return;
}

void disp_init() {
    display.init();
    display.setRotation(1);
    u8g2Fonts.begin(display);
    return;
}

void disp_clear() {
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
    } while(display.nextPage());
    display.hibernate();
    return;
}

void disp_update(uint8_t refresh /*=0*/) {
    static uint16_t last_year;
    static uint8_t last_month;
    static uint8_t last_day;
    static uint8_t last_hour;
    static uint8_t last_minute;
    static op_meridiem_t last_meridiem;


    uint8_t date_diff = (op_day != last_day || op_month != last_month || op_year != last_year);
    uint8_t time_diff = (op_minute != last_minute || op_hour != last_hour || op_meridiem != last_meridiem);

    if (!time_diff && !date_diff && !refresh) { // No change, don't do anything
        return;
    }

    if (date_diff || refresh) {
        // Update Date
        sprintf(date_buf, "%s, %s %d, %d", 
                dayStr(clock_dayofweek(op_year, op_month, op_day)), monthStr(op_month), 
                op_day, op_year);
        u8g2Fonts.setFont(SMALL_FONT);
        dateWidth = u8g2Fonts.getUTF8Width(date_buf);
        dateX = (DISP_WIDTH-dateWidth)/2;
    }
    // Update Time
    if (op_meridiem != MIL) {
        sprintf(time_buf, "%d:%02d", op_hour, op_minute);
    }
    else {
        sprintf(time_buf, "%02d:%02d", op_hour, op_minute);
    }
    u8g2Fonts.setFont(BIG_FONT);
    timeWidth = u8g2Fonts.getUTF8Width(time_buf);
    if (op_hour < 10 || op_meridiem == MIL) {
        timeX = (DISP_WIDTH-timeWidth)/2; // Center
    }
    else { 
        timeX = (TIME12_AREA_WIDTH-timeWidth)/2 - 2;
    }

    if (date_diff || refresh) {
        _drawFull();
    }
    else {
        _drawTime();
    }

    last_year = op_year;
    last_month = op_month;
    last_day = op_day;
    last_hour = op_hour;
    last_minute = op_minute;
    last_meridiem = op_meridiem;
    return;
}

void disp_setDoT(uint8_t dateTime_n) {
    uint16_t date_fg = dateTime_n ? GxEPD_WHITE : GxEPD_BLACK;
    uint16_t date_bg = dateTime_n ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t time_fg = dateTime_n ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t time_bg = dateTime_n ? GxEPD_WHITE : GxEPD_BLACK;
    uint16_t rect_x = dateTime_n ? SET_DOT_DATE_CURSOR_X - 1 : SET_DOT_TIME_CURSOR_X - 1;
    uint16_t rect_w = dateTime_n ? SMALL_DATE_WIDTH + 2 : SMALL_TIME_WIDTH + 2;

    display.setPartialWindow(SET_DOT_DATE_CURSOR_X-1, SET_AREA_Y,
            SET_DOT_WIDTH+2, SET_AREA_HEIGHT);
    
    u8g2Fonts.setFont(SMALL_FONT);
    display.firstPage();
    do {
            display.fillScreen(GxEPD_WHITE);

            display.writeFillRect(rect_x, SET_AREA_Y,
                    rect_w, SET_AREA_HEIGHT,
                    GxEPD_BLACK);
            
            u8g2Fonts.setForegroundColor(date_fg);
            u8g2Fonts.setBackgroundColor(date_bg);
            u8g2Fonts.setCursor(SET_DOT_DATE_CURSOR_X, SET_CURSOR_Y);
            u8g2Fonts.print(DATE_STR); 

            u8g2Fonts.setForegroundColor(time_fg);
            u8g2Fonts.setBackgroundColor(time_bg);
            u8g2Fonts.setCursor(SET_DOT_TIME_CURSOR_X, SET_CURSOR_Y);
            u8g2Fonts.print(TIME_STR); 
    } while (display.nextPage());
    
    return;
}

void disp_setTime(op_meridiem_t meridiem, int8_t h, int8_t m) {
    char* meridiem_str = (char*) MERIDIEM_STR[(int)meridiem];
    sprintf(set_buf2, "%02d", h);
    sprintf(set_buf3, "%02d", m);

    display.setFullWindow();

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        u8g2Fonts.setCursor(SET_TIME_TITLE_CURSOR_X, SET_TITLE_CURSOR_Y);
        u8g2Fonts.print(TIME_STR);

        u8g2Fonts.setCursor(SET_TIME_MERIDIEM_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(meridiem_str); 

        u8g2Fonts.setCursor(SET_TIME_HOUR_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf2); 

        u8g2Fonts.setCursor(SET_TIME_COLON_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(':'); 

        u8g2Fonts.setCursor(SET_TIME_MINUTE_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf3); 
    } while(display.nextPage());

    return;
}

void disp_setTimeMeridiem(op_meridiem_t meridiem, uint8_t sel) {
    uint16_t bg = sel ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t fg = sel ? GxEPD_WHITE : GxEPD_BLACK;

    char* meridiem_str = (char*) MERIDIEM_STR[(int)meridiem];
    
    display.setPartialWindow(SET_TIME_MERIDIEM_CURSOR_X-1, SET_AREA_Y,
        SMALL_MERIDIEM_WIDTH+2, SET_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);
    display.firstPage();
    do {
        display.fillScreen(bg);

        u8g2Fonts.setCursor(SET_TIME_MERIDIEM_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(meridiem_str); 
    } while(display.nextPage());

    return;
}

void disp_setTimeHour(int8_t h, uint8_t sel) {
    uint16_t bg = sel ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t fg = sel ? GxEPD_WHITE : GxEPD_BLACK;

    sprintf(set_buf2, "%02d", h);
    
    display.setPartialWindow(SET_TIME_HOUR_CURSOR_X-1, SET_AREA_Y,
        SMALL_HOUR_WIDTH+2, SET_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);
    display.firstPage();
    do {
        display.fillScreen(bg);

        u8g2Fonts.setCursor(SET_TIME_HOUR_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf2); 
    } while(display.nextPage());

    return;
}

void disp_setTimeMinute(int8_t m, uint8_t sel) {
    uint16_t bg = sel ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t fg = sel ? GxEPD_WHITE : GxEPD_BLACK;

    sprintf(set_buf3, "%02d", m);
    
    display.setPartialWindow(SET_TIME_MINUTE_CURSOR_X-1, SET_AREA_Y,
        SMALL_DAY_WIDTH+2, SET_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);
    display.firstPage();
    do {
        display.fillScreen(bg);

        u8g2Fonts.setCursor(SET_TIME_MINUTE_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf3); 
    } while(display.nextPage());

    return;
}

void disp_setDate(uint16_t yr, int8_t m, int8_t d) {
    sprintf(set_buf1, "%04d", yr);
    sprintf(set_buf2, "%02d", m);
    sprintf(set_buf3, "%02d", d);

    display.setFullWindow();

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        u8g2Fonts.setCursor(SET_DATE_TITLE_CURSOR_X, SET_TITLE_CURSOR_Y);
        u8g2Fonts.print(DATE_STR);

        u8g2Fonts.setCursor(SET_DATE_YEAR_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf1); 

        u8g2Fonts.setCursor(SET_DATE_DASH1_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print('-'); 

        u8g2Fonts.setCursor(SET_DATE_MONTH_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf2); 

        u8g2Fonts.setCursor(SET_DATE_DASH2_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print('-'); 

        u8g2Fonts.setCursor(SET_DATE_DAY_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf3); 
    } while(display.nextPage());

    return;
}

void disp_setDateYear(uint16_t yr, uint8_t sel) {
    uint16_t bg = sel ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t fg = sel ? GxEPD_WHITE : GxEPD_BLACK;

    sprintf(set_buf1, "%04d", yr);
    
    display.setPartialWindow(SET_DATE_YEAR_CURSOR_X-1, SET_AREA_Y,
        SMALL_YEAR_WIDTH+2, SET_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);
    display.firstPage();
    do {
        display.fillScreen(bg);

        u8g2Fonts.setCursor(SET_DATE_YEAR_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf1); 
    } while(display.nextPage());

    return;
}

void disp_setDateMonth(int8_t m, uint8_t sel) {
    uint16_t bg = sel ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t fg = sel ? GxEPD_WHITE : GxEPD_BLACK;

    sprintf(set_buf2, "%02d", m);
    
    display.setPartialWindow(SET_DATE_MONTH_CURSOR_X-1, SET_AREA_Y,
        SMALL_MONTH_WIDTH+2, SET_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);
    display.firstPage();
    do {
        display.fillScreen(bg);

        u8g2Fonts.setCursor(SET_DATE_MONTH_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf2); 
    } while(display.nextPage());

    return;
}

void disp_setDateDay(int8_t d, uint8_t sel) {
    uint16_t bg = sel ? GxEPD_BLACK : GxEPD_WHITE;
    uint16_t fg = sel ? GxEPD_WHITE : GxEPD_BLACK;

    sprintf(set_buf3, "%02d", d);
    
    display.setPartialWindow(SET_DATE_DAY_CURSOR_X-1, SET_AREA_Y,
        SMALL_DAY_WIDTH+2, SET_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(fg);
    u8g2Fonts.setBackgroundColor(bg);
    display.firstPage();
    do {
        display.fillScreen(bg);

        u8g2Fonts.setCursor(SET_DATE_DAY_CURSOR_X, SET_CURSOR_Y);
        u8g2Fonts.print(set_buf3); 
    } while(display.nextPage());

    return;
}

void disp_setSave() {
    display.setPartialWindow(SET_SAVE_CURSOR_X-1, SET_SAVE_AREA_Y,
            SET_SAVE_WIDTH+2, SET_SAVE_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(GxEPD_WHITE);
    u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_BLACK);
        u8g2Fonts.setCursor(SET_SAVE_CURSOR_X, SET_SAVE_CURSOR_Y);
        u8g2Fonts.print(SAVE_STR);
    } while (display.nextPage());
}

void disp_setSaving() {
    // Use SAVE_WIDTH instead of SAVING_WIDTH, since "Set" is wider than "..."
    display.setPartialWindow(SET_SAVE_CURSOR_X-1, SET_SAVE_AREA_Y,
            SET_SAVE_WIDTH+2, SET_SAVE_AREA_HEIGHT);

    u8g2Fonts.setFont(SMALL_FONT);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        u8g2Fonts.setCursor(SET_SAVING_CURSOR_X, SET_SAVE_CURSOR_Y);
        u8g2Fonts.print(SAVING_STR);
    } while (display.nextPage());
}
