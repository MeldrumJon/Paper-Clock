#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

// 90deg rotation
#define ROTATION U8G2_R0

#define CS 10
#define DC 8
#define RST 9

U8G2_IL3820_296X128_2_4W_HW_SPI u8g2(ROTATION, CS, DC, RST);	

void setup(void) {
  u8g2.begin();
}

void loop(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "Hello World!");
  } while ( u8g2.nextPage() );
  delay(1000);
}
