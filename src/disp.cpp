#include "disp.h"
#include <avr/pgmspace.h>
#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <TimeLib.h>
#include "clock.h"

// Fonts
#define DATE_FONT_HEIGHT 13
#define MERIDIEM_FONT_HEIGHT 13
#define TIME_FONT_HEIGHT 69
// Display
#define DISP_WIDTH 296
#define DISP_HEIGHT 128
#define MERIDIEM_WIDTH 25
#define MERIDIEM_HEIGHT MERIDIEM_FONT_HEIGHT
#define MERIDIEM_PADDING 7
#define MERIDIEM_CURSOR_X (DISP_WIDTH - (MERIDIEM_WIDTH + MERIDIEM_PADDING))
#define MERIDIEM_CURSOR_Y (DISP_HEIGHT - MERIDIEM_PADDING)
#define MERIDIEM_AREA_WIDTH (MERIDIEM_WIDTH + MERIDIEM_PADDING*2 - 2)
#define MERIDIEM_AREA_HEIGHT (MERIDIEM_HEIGHT + MERIDIEM_PADDING*2)
#define MERIDIEM_AREA_X (DISP_WIDTH - MERIDIEM_AREA_WIDTH)
#define MERIDIEM_AREA_Y (DISP_HEIGHT - MERIDIEM_AREA_HEIGHT)

#define TIME_DATE_SEP 20
#define TIME_DATE_HEIGHT (TIME_FONT_HEIGHT + DATE_FONT_HEIGHT + TIME_DATE_SEP)

#define TIME_Y ((DISP_HEIGHT + TIME_DATE_HEIGHT)/2)
#define TIME_AREA_WIDTH (DISP_WIDTH - MERIDIEM_AREA_WIDTH)
#define TIME_AREA_HEIGHT ((DISP_HEIGHT - TIME_Y) + TIME_FONT_HEIGHT + 2)
#define TIME_AREA_X 0
#define TIME_AREA_Y (TIME_Y - TIME_FONT_HEIGHT - 1)

#define DATE_Y (TIME_Y - TIME_FONT_HEIGHT - TIME_DATE_SEP)

#define MAX_DISPAY_BUFFER_SIZE 800 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPAY_BUFFER_SIZE / (EPD::WIDTH / 8))

#define BMASK_CLK (0x1<<5)
#define BMASK_MOSI (0x1<<3)
#define BMASK_SS (0x1<<2)
#define BMASK_RST (0x1<<1)
#define BMASK_DC (0x1<<0)
#define DMASK_BUSY (0x1<<7)
#define DMASK_PWR (0x1<<5)

GxEPD2_BW<GxEPD2_290, MAX_HEIGHT(GxEPD2_290)> display(GxEPD2_290(/*CS=10*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));
//GxEPD2_AVR_BW display(GxEPD2::GDEH029A1, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
// Fonts
extern const uint8_t u8g2_font_droidserif_96pt[] U8G2_FONT_SECTION("u8g2_font_droidserif_96pt");
extern const uint8_t u8g2_font_timR14[] U8G2_FONT_SECTION("u8g2_font_timR14");

static const char AM_STR[] = "AM";
static const char PM_STR[] = "PM";
static char date_buf[31]; // Longest date string: "Wednesday, September 20, 2019" = 30 chars + \0
static char time_buf[6]; // Longest time "12:00" = 4 chars + \0

static uint8_t dateX;
static uint8_t timeX;

static enum updateMode_t {
    FULL,
    TIME
} updateMode;

static void _drawFull(uint8_t isPM) {
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
    } while (display.nextPage());
}

// Not currently used because of refresh on 00, 15, 30, 45
//static void _drawTimeMeridiem(uint8_t isPM) {
//    display.setPartialWindow(0, TIME_AREA_Y,
//            DISP_WIDTH, TIME_AREA_HEIGHT);
//    display.firstPage();
//    do {
//        // Background Color
//        display.fillScreen(GxEPD_WHITE);
//
//        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
//        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
//   
//        // Draw time string
//        u8g2Fonts.setFont(u8g2_font_droidserif_96pt);
//        u8g2Fonts.setCursor(timeX, TIME_Y);
//        u8g2Fonts.print(time_buf);
//        
//        // Draw Meridian
//        u8g2Fonts.setFont(u8g2_font_timR14);
//        u8g2Fonts.setCursor(MERIDIEM_CURSOR_X, MERIDIEM_CURSOR_Y);
//        if (isPM) {
//            display.writeFillRect(MERIDIEM_AREA_X, MERIDIEM_AREA_Y,
//                    MERIDIEM_AREA_WIDTH, MERIDIEM_AREA_HEIGHT,
//                    GxEPD_BLACK);
//            u8g2Fonts.setForegroundColor(GxEPD_WHITE);
//            u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
//            u8g2Fonts.print(PM_STR);
//        }
//        else {
//            display.writeFastHLine(MERIDIEM_AREA_X, MERIDIEM_AREA_Y, MERIDIEM_AREA_WIDTH, GxEPD_BLACK);
//            display.writeFastVLine(MERIDIEM_AREA_X, MERIDIEM_AREA_Y, MERIDIEM_AREA_HEIGHT, GxEPD_BLACK);
//            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
//            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
//            u8g2Fonts.print(AM_STR);
//        }
//    } while (display.nextPage());
//
//}

static void _drawTime() {
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_droidserif_96pt);

    display.setPartialWindow(TIME_AREA_X, TIME_AREA_Y,
            TIME_AREA_WIDTH, TIME_AREA_HEIGHT);
    display.firstPage();
    do {
        // Background Color
        display.fillScreen(GxEPD_WHITE);
        // Draw Time String
        u8g2Fonts.setCursor(timeX, TIME_Y);
        u8g2Fonts.print(time_buf);
    } while (display.nextPage());
}

void disp_init() {
    //DDRD |= (DMASK_PWR);
    //PORTD |= (DMASK_PWR); // Enable display power.
    //// TODO: do we need a delay? how long?
    //_delay_ms(1000);
    display.init();
    display.setRotation(1);
    u8g2Fonts.begin(display);
}

void disp_off() {
//    // Make display pins input, without pullup (high-impedance)
//    DDRB &= ~(BMASK_DC | BMASK_RST | BMASK_SS | BMASK_MOSI | BMASK_CLK);
//    PORTB &= ~(BMASK_DC | BMASK_RST | BMASK_SS | BMASK_MOSI | BMASK_CLK); 
//    // DMASK_BUSY already set to input without pullup
//
//    // DMASK_PWR stays output
//    PORTD &= ~(DMASK_PWR);
}

void disp_on() {
//    // DMASK_PWR stays output
//    PORTD |= (DMASK_PWR);
//
//    // DMASK_BUSY already set to input without pullup
//    // DC, RST, SS, MOSI, CLK should be outputs
//    DDRD |= (BMASK_DC | BMASK_RST | BMASK_SS | BMASK_MOSI | BMASK_CLK);
//    // DC, RST, SS should be high, others should already be low after disp_off
//    PORTB |= (BMASK_DC | BMASK_RST | BMASK_SS);
//
//    // TODO: do we need a delay? how long?
//    _delay_ms(1000);
//    display.reinit();
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
    uint8_t h = hourFormat12(t);
    uint8_t m = minute(t);
    uint8_t pm = isPM(t);

    uint16_t dateWidth;
    uint16_t timeWidth;

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
            sprintf(time_buf, "%d:%02d", h, m);
            u8g2Fonts.setFont(u8g2_font_droidserif_96pt);
            timeWidth = u8g2Fonts.getUTF8Width(time_buf);
            if (h >= 10) { 
                timeX = (TIME_AREA_WIDTH-timeWidth)/2 - 2;
            }
            else {
                timeX = (DISP_WIDTH-timeWidth)/2;
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
    display.hibernate();

    last_year = y;
    last_month = mo;
    last_day = d;
    last_hour = h;
    last_minute = m;
    last_isPM = pm;
    return;
}
