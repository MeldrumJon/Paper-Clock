#include "disp.h"
#include <avr/pgmspace.h>
#define DISABLE_DIAGNOSTIC_OUTPUT
#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <TimeLib.h>
#include "clock.h"
#include "setting.h"

// Fonts
#define DATE_FONT_HEIGHT 13
#define MERIDIEM_FONT_HEIGHT 13
#define TIME_FONT_HEIGHT 69
// Display
#define DISP_WIDTH 296
#define DISP_HEIGHT 128
#define MERIDIEM_WIDTH 25+1
#define MERIDIEM_HEIGHT MERIDIEM_FONT_HEIGHT+1
#define MERIDIEM_PADDING 7
#define MERIDIEM_CURSOR_X (DISP_WIDTH - (MERIDIEM_WIDTH + MERIDIEM_PADDING))
#define MERIDIEM_CURSOR_Y (DISP_HEIGHT - MERIDIEM_PADDING)
#define MERIDIEM_AREA_WIDTH (MERIDIEM_WIDTH + MERIDIEM_PADDING*2 - 2)
#define MERIDIEM_AREA_HEIGHT (MERIDIEM_HEIGHT + MERIDIEM_PADDING*2)
#define MERIDIEM_AREA_X (DISP_WIDTH - MERIDIEM_AREA_WIDTH)
#define MERIDIEM_AREA_Y (DISP_HEIGHT - MERIDIEM_AREA_HEIGHT)

#define TIME_DATE_SEP 20
#define TIME_DATE_HEIGHT (TIME_FONT_HEIGHT + DATE_FONT_HEIGHT + TIME_DATE_SEP)

#define TIME_Y ((DISP_HEIGHT + TIME_DATE_HEIGHT)/2-1)
#define TIME_AREA_WIDTH (DISP_WIDTH - (MERIDIEM_AREA_WIDTH+1))
#define TIME_AREA_HEIGHT ((DISP_HEIGHT - TIME_Y) + TIME_FONT_HEIGHT + 2)
#define TIME_AREA_X 0
#define TIME_AREA_Y (TIME_Y - TIME_FONT_HEIGHT - 1)

#define TIME24_AREA_WIDTH (DISP_WIDTH)

#define DATE_Y (TIME_Y - TIME_FONT_HEIGHT - TIME_DATE_SEP+1)


#define SETTING_Y ((DISP_HEIGHT+DATE_FONT_HEIGHT)/2)
#define SETTING_AREA_Y ((DISP_HEIGHT-DATE_FONT_HEIGHT)/2 - 1)
#define SETTING_AREA_HEIGHT (DATE_FONT_HEIGHT + 2)

#define SETTING_CONFIRM_AREA_Y (SETTING_AREA_Y+SETTING_AREA_HEIGHT+16)
#define SETTING_CONFIRM_AREA_HEIGHT (DATE_FONT_HEIGHT + 2)
#define SETTING_CONFIRM_Y (SETTING_CONFIRM_AREA_Y+DATE_FONT_HEIGHT-1)

#define SETTING_DATE_TIME_SEP 56
#define SETTING_DATE_SEP 12

#define SETTING_NUM_X (DISP_WIDTH/2 + 4)

#define SETTING_AREA_X (SETTING_NUM_X - 1)
#define SETTING_AREA_WIDTH 40

#define BMASK_CLK (0x1<<5)
#define BMASK_MOSI (0x1<<3)
#define BMASK_SS (0x1<<2)
#define BMASK_RST (0x1<<1)
#define BMASK_DC (0x1<<0)
#define DMASK_BUSY (0x1<<7)
#define DMASK_PWR (0x1<<5)

// Display
#define MAX_DISPAY_BUFFER_SIZE 800 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8))
GxEPD2_BW<GxEPD2_290, MAX_HEIGHT(GxEPD2_290)> display(GxEPD2_290(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));

// Fonts
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
extern const uint8_t u8g2_font_droidserif_96pt[] U8G2_FONT_SECTION("u8g2_font_droidserif_96pt");
extern const uint8_t u8g2_font_timR14[] U8G2_FONT_SECTION("u8g2_font_timR14");

// Variables
static const char AM_STR[] = "AM";
static const char PM_STR[] = "PM";
static char date_buf[31]; // Longest date string: "Wednesday, September 20, 2019" = 30 chars + \0
static char time_buf[6]; // Longest time "12:00" = 4 chars + \0

static uint8_t dateX;
static uint8_t timeX;

// Strings
static const char DATE_SET_STR[] = "DATE";
static const char TIME_SET_STR[] = "TIME";
static const char DATE_SEP_STR[] = "-";

static enum updateMode_t {
    FULL,
    TIME
} updateMode;

static void _drawFull(uint8_t isPM) {
    setting_meridiem_t meridiem = setting_getMeridiem();
    display.setFullWindow();
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
        u8g2Fonts.setCursor(timeX, TIME_Y);
        u8g2Fonts.print(time_buf);
        
        // Draw Meridian
        if (meridiem != MILITARY) {
            u8g2Fonts.setFont(u8g2_font_timR14);
            u8g2Fonts.setCursor(MERIDIEM_CURSOR_X, MERIDIEM_CURSOR_Y);
            if (isPM) {
                display.writeFillRect(MERIDIEM_AREA_X, MERIDIEM_AREA_Y,
                        MERIDIEM_AREA_WIDTH, MERIDIEM_AREA_HEIGHT,
                        GxEPD_BLACK);
                u8g2Fonts.setForegroundColor(GxEPD_WHITE);
                u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
                u8g2Fonts.print(PM_STR);
            }
            else {
                display.writeFastHLine(MERIDIEM_AREA_X, MERIDIEM_AREA_Y, MERIDIEM_AREA_WIDTH, GxEPD_BLACK);
                display.writeFastVLine(MERIDIEM_AREA_X, MERIDIEM_AREA_Y, MERIDIEM_AREA_HEIGHT, GxEPD_BLACK);
                u8g2Fonts.setForegroundColor(GxEPD_BLACK);
                u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
                u8g2Fonts.print(AM_STR);
            }
        }
    } while (display.nextPage());
    display.hibernate();
}

static void _drawTime() {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_droidserif_96pt);
    
    if (setting_getMeridiem() == MILITARY) {
        display.setPartialWindow(TIME_AREA_X, TIME_AREA_Y,
                TIME24_AREA_WIDTH, TIME_AREA_HEIGHT);
    }
    else {
        display.setPartialWindow(TIME_AREA_X, TIME_AREA_Y,
                TIME_AREA_WIDTH, TIME_AREA_HEIGHT);
    }
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        // Draw Time String
        u8g2Fonts.setCursor(timeX, TIME_Y);
        u8g2Fonts.print(time_buf);
    } while (display.nextPage());
    display.hibernate();
}

void disp_init() {
    display.init();
    display.setRotation(1);
    u8g2Fonts.begin(display);
}

void disp_clear() {
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
    } while(display.nextPage());
    display.hibernate();
}

void disp_update(time_t t, uint8_t refresh /*=0*/) {
    static uint16_t last_year;
    static uint8_t last_month;
    static uint8_t last_day;
    static uint8_t last_hour;
    static uint8_t last_minute;
    static uint8_t last_isPM;

    uint16_t y = year(t);
    uint8_t mo = month(t);
    uint8_t d = day(t);
    uint8_t h;
    uint8_t m = minute(t);
    uint8_t pm = isPM(t);

    uint16_t dateWidth;
    uint16_t timeWidth;

    setting_meridiem_t meridiem = setting_getMeridiem();

    if (meridiem == MILITARY) {
        h = hour(t);
    }
    else {
        h = hourFormat12(t);
    }

    // Full update
    if ( (m != last_minute && (m % 15) == 0) || pm != last_isPM // Should do full update every few partials
            || d != last_day || mo != last_month || y != last_year || refresh)
    {
        updateMode = FULL;
    }
    else if (m != last_minute || h != last_hour) { // Partial update
        updateMode = TIME;
    }
    else {
        return;
    }

    switch (updateMode) {
        case FULL:
            // Update Date
            sprintf(date_buf, "%s, %s %d, %d", dayStr(weekday(t)),
                    monthStr(mo), d, y);
            u8g2Fonts.setFont(u8g2_font_timR14);
            dateWidth = u8g2Fonts.getUTF8Width(date_buf);
            dateX = (DISP_WIDTH-dateWidth)/2;
        case TIME:
            // Update Time
            if (meridiem != MILITARY) {
                sprintf(time_buf, "%d:%02d", h, m);
            }
            else {
                sprintf(time_buf, "%02d:%02d", h, m);
            }
            u8g2Fonts.setFont(u8g2_font_droidserif_96pt);
            timeWidth = u8g2Fonts.getUTF8Width(time_buf);
            if (h < 10 || meridiem == MILITARY) {
                timeX = (DISP_WIDTH-timeWidth)/2;
            }
            else { 
                timeX = (TIME_AREA_WIDTH-timeWidth)/2 - 2;
            }
    }

    switch (updateMode) {
        case FULL:
            _drawFull(pm);
            break;
        case TIME:
            _drawTime();
            break;
    }

    last_year = y;
    last_month = mo;
    last_day = d;
    last_hour = h;
    last_minute = m;
    last_isPM = pm;
    return;
}

void disp_dateOrTime(uint8_t dateTime_n, uint8_t firstRun) {
    u8g2Fonts.setFont(u8g2_font_timR14);
    uint16_t date_width = u8g2Fonts.getUTF8Width(DATE_SET_STR);
    uint16_t time_width = u8g2Fonts.getUTF8Width(TIME_SET_STR); 
    uint16_t width = date_width + time_width + SETTING_DATE_TIME_SEP;
    uint16_t date_x = (DISP_WIDTH-width)/2;
    uint16_t time_x = date_x+date_width+SETTING_DATE_TIME_SEP;
    if (firstRun) {
        display.setFullWindow();
    }
    else {
        display.setPartialWindow(date_x-1, SETTING_AREA_Y,
                width+2, SETTING_AREA_HEIGHT);
    }

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        if (dateTime_n) {
            display.writeFillRect(date_x-1, SETTING_AREA_Y,
                    date_width+2, SETTING_AREA_HEIGHT,
                    GxEPD_BLACK);
            u8g2Fonts.setForegroundColor(GxEPD_WHITE);
            u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
            u8g2Fonts.setCursor(date_x, SETTING_Y);
            u8g2Fonts.print(DATE_SET_STR); 

            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.setCursor(time_x, SETTING_Y);
            u8g2Fonts.print(TIME_SET_STR); 
        }
        else {
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.setCursor(date_x, SETTING_Y);
            u8g2Fonts.print(DATE_SET_STR); 

            display.writeFillRect(time_x-1, SETTING_AREA_Y,
                    time_width+2, SETTING_AREA_HEIGHT,
                    GxEPD_BLACK);
            u8g2Fonts.setForegroundColor(GxEPD_WHITE);
            u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
            u8g2Fonts.setCursor(time_x, SETTING_Y);
            u8g2Fonts.print(TIME_SET_STR); 
        }
    } while (display.nextPage());
}

void disp_setDate(uint16_t yr, uint8_t m, uint8_t d, drawDate_t select_ymdn, 
        uint8_t update, uint8_t firstRun) {
    u8g2Fonts.setFont(u8g2_font_timR14);
    static char yr_str[5];
    static char m_str[3];
    static char d_str[3];
    sprintf(yr_str, "%04d", yr);
    sprintf(m_str, "%02d", m);
    sprintf(d_str, "%02d", d);
    uint16_t sep_width = u8g2Fonts.getUTF8Width(DATE_SEP_STR);
    uint16_t yr_width = u8g2Fonts.getUTF8Width(yr_str);
    uint16_t m_width = u8g2Fonts.getUTF8Width(m_str);
    uint16_t d_width = u8g2Fonts.getUTF8Width(d_str);
    uint16_t width = sep_width*2 + yr_width + m_width + d_width + SETTING_DATE_SEP*4;
    uint16_t yr_x = (DISP_WIDTH-width)/2;
    uint16_t sep1_x = yr_x + yr_width + SETTING_DATE_SEP;
    uint16_t m_x = sep1_x + sep_width + SETTING_DATE_SEP;
    uint16_t sep2_x = m_x + m_width + SETTING_DATE_SEP;
    uint16_t d_x = sep2_x + sep_width + SETTING_DATE_SEP;
    if (firstRun) {
        display.setFullWindow();
    }
    else {
        if (update) {
            switch(select_ymdn) {
                case DRAW_YEAR:
                    display.setPartialWindow(yr_x-1, SETTING_AREA_Y,
                            yr_width+2, SETTING_AREA_HEIGHT);
                    break;
                case DRAW_MONTH:
                    display.setPartialWindow(m_x-1, SETTING_AREA_Y,
                            m_width+2, SETTING_AREA_HEIGHT);
                    break;
                case DRAW_DAY:
                    display.setPartialWindow(d_x-1, SETTING_AREA_Y,
                            d_width+2, SETTING_AREA_HEIGHT);
                    break;
                deafult:
                    display.setPartialWindow(yr_x-1, SETTING_AREA_Y,
                            width+2, SETTING_AREA_HEIGHT);
                    break;
            }
        }
        else {
            display.setPartialWindow(yr_x-1, SETTING_AREA_Y,
                    width+2, SETTING_AREA_HEIGHT);
        }
    }
    display.firstPage();
    do {
        if (!update) {
            display.fillScreen(GxEPD_WHITE);
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        }
        switch(select_ymdn) {
            case DRAW_YEAR:
                if (!update) {
                    u8g2Fonts.setCursor(m_x, SETTING_Y);
                    u8g2Fonts.print(m_str); 
                    u8g2Fonts.setCursor(d_x, SETTING_Y);
                    u8g2Fonts.print(d_str); 
                    display.writeFillRect(yr_x-1, SETTING_AREA_Y,
                            yr_width+2, SETTING_AREA_HEIGHT,
                            GxEPD_BLACK);
                }
                else {
                    display.fillScreen(GxEPD_BLACK);
                }
                u8g2Fonts.setForegroundColor(GxEPD_WHITE);
                u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
                u8g2Fonts.setCursor(yr_x, SETTING_Y);
                u8g2Fonts.print(yr_str); 

                break;
            case DRAW_MONTH:
                if (!update) {
                    u8g2Fonts.setCursor(yr_x, SETTING_Y);
                    u8g2Fonts.print(yr_str); 
                    u8g2Fonts.setCursor(d_x, SETTING_Y);
                    u8g2Fonts.print(d_str); 
                    display.writeFillRect(m_x-1, SETTING_AREA_Y,
                            m_width+2, SETTING_AREA_HEIGHT,
                            GxEPD_BLACK);
                }
                else {
                    display.fillScreen(GxEPD_BLACK);
                }
                u8g2Fonts.setForegroundColor(GxEPD_WHITE);
                u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
                u8g2Fonts.setCursor(m_x, SETTING_Y);
                u8g2Fonts.print(m_str); 

                break;
            case DRAW_DAY:
                if (!update) {
                    u8g2Fonts.setCursor(yr_x, SETTING_Y);
                    u8g2Fonts.print(yr_str); 
                    u8g2Fonts.setCursor(m_x, SETTING_Y);
                    u8g2Fonts.print(m_str); 
                    display.writeFillRect(d_x-1, SETTING_AREA_Y,
                            d_width+2, SETTING_AREA_HEIGHT,
                            GxEPD_BLACK);
                }
                else {
                    display.fillScreen(GxEPD_BLACK);
                }
                u8g2Fonts.setForegroundColor(GxEPD_WHITE);
                u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
                u8g2Fonts.setCursor(d_x, SETTING_Y);
                u8g2Fonts.print(d_str); 

                break;
            default:
                display.fillScreen(GxEPD_WHITE);
                u8g2Fonts.setForegroundColor(GxEPD_BLACK);
                u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
                u8g2Fonts.setCursor(yr_x, SETTING_Y);
                u8g2Fonts.print(yr_str); 
                u8g2Fonts.setCursor(m_x, SETTING_Y);
                u8g2Fonts.print(m_str); 
                u8g2Fonts.setCursor(d_x, SETTING_Y);
                u8g2Fonts.print(d_str); 
                break;
        }
    } while (display.nextPage());
}

void disp_setting(setting_t setting) {
    switch (setting) {
        case YEAR:
            sprintf(date_buf, "Year:");
            break;
        case MONTH:
            sprintf(date_buf, "Month:");
            break;
        case DAY:
            sprintf(date_buf, "Day:");
            break;
        case MERIDIEM:
            sprintf(date_buf, "Meridiem:");
            break;
        case HOUR:
            sprintf(date_buf, "Hours:");
            break;
        case MINUTE:
            sprintf(date_buf, "Minute:");
            break;
    }
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_timR14);
    uint16_t width = u8g2Fonts.getUTF8Width(date_buf);
    uint16_t settingX = (DISP_WIDTH/2-width);
    display.setFullWindow();
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        // Draw Time String
        u8g2Fonts.setCursor(settingX, SETTING_Y);
        u8g2Fonts.print(date_buf);
    } while (display.nextPage());
}

void disp_showSet(uint16_t num) {
    sprintf(time_buf, "%d", num);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_timR14);
    display.setPartialWindow(SETTING_AREA_X, SETTING_AREA_Y,
            SETTING_AREA_WIDTH, SETTING_AREA_HEIGHT);
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        // Draw Time String
        u8g2Fonts.setCursor(SETTING_NUM_X, SETTING_Y);
        u8g2Fonts.print(time_buf);
    } while (display.nextPage());
    return;
}

void disp_showMeridiem() {
    setting_meridiem_t meridiem = setting_getMeridiem();
    switch (meridiem) {
        case AM:
            sprintf(time_buf, "AM");
            break;
        case PM:
            sprintf(time_buf, "PM");
            break;
        case MILITARY:
            sprintf(time_buf, "24");
            break;
    }
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_timR14);
    display.setPartialWindow(SETTING_AREA_X, SETTING_AREA_Y,
            SETTING_AREA_WIDTH, SETTING_AREA_HEIGHT);
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        // Draw Time String
        u8g2Fonts.setCursor(SETTING_NUM_X, SETTING_Y);
        u8g2Fonts.print(time_buf);
    } while (display.nextPage());
    return;
}

void disp_confirm(uint8_t isSetting) {
    u8g2Fonts.setFont(u8g2_font_timR14);
    if (isSetting) {
        sprintf(date_buf, "Setting...");
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    }
    else {
        sprintf(date_buf, "Set");
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
    }
    uint16_t width = u8g2Fonts.getUTF8Width(date_buf);
    uint16_t x = (DISP_WIDTH-width)/2;
    display.setPartialWindow(x-1, SETTING_CONFIRM_AREA_Y,
            width+2, SETTING_CONFIRM_AREA_HEIGHT);
    display.firstPage();
    do {
        if (isSetting) {
            display.fillScreen(GxEPD_WHITE);
        }
        else {
            display.fillScreen(GxEPD_BLACK);
        }
        u8g2Fonts.setCursor(x, SETTING_CONFIRM_Y);
        u8g2Fonts.print(date_buf);
    } while (display.nextPage());
}
