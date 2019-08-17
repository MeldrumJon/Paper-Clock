#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

// 90deg rotation
#define ROTATION U8G2_R0

#define CS 10
#define DC 8
#define RST 9

u8g2_t u8g2;

int main(void) {
    
}

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
