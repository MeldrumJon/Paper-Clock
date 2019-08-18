// GxEPD2_AVR_SD_BitmapExample : test example for e-Paper displays from Waveshare and from Dalian Good Display Inc.
//
// Created by Jean-Marc Zingg based on demo code from Good Display,
// available on http://www.good-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// The e-paper displays are available from:
//
// https://www.aliexpress.com/store/product/Wholesale-1-54inch-E-Ink-display-module-with-embedded-controller-200x200-Communicate-via-SPI-interface-Supports/216233_32824535312.html
//
// http://www.buy-lcd.com/index.php?route=product/product&path=2897_8363&product_id=35120
// or https://www.aliexpress.com/store/product/E001-1-54-inch-partial-refresh-Small-size-dot-matrix-e-paper-display/600281_32815089163.html
//

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Dispay ePaper for Arduino : https://forum.arduino.cc/index.php?topic=436411.0

// mapping suggestion from Waveshare SPI e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

// mapping suggestion for AVR, UNO, NANO etc.
// BUSY -> 7, RST -> 9, DC -> 8, CS-> 10, CLK -> 13, DIN -> 11
//
// **** NOTE that the mapping suggestion may need modification depending on SD board used! ****
// ********************************************************************************************
//
//#define SD_CS SS  // e.g. for RobotDyn Wemos D1 mini SD board
//#define EPD_CS D1 // alternative I use with RobotDyn Wemos D1 mini SD board

#include <GxEPD2_AVR_BW.h>
#include <GxEPD2_AVR_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// include SdFat for FAT32 support with long filenames; available through Library Manager
#include <SdFat.h>

#if defined(SdFat_h)
SdFat SD;
#define FileClass SdFile
#define position curPosition
#define seek seekSet
#else
#include <SD.h>
#define FileClass File
#undef SdFat_h
#endif

#if defined(__AVR)
#define SD_CS 6  // adapt to your mapping
#define EPD_CS SS // adapt to your mapping
// select one and adapt to your mapping
//GxEPD2_AVR_BW display(GxEPD2::GDEP015OC1, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDE0213B1, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDEH029A1, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDEW027W3, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDEW042T2, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_BW display(GxEPD2::GDEW075T8, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
// 3-color e-papers
//GxEPD2_AVR_3C display(GxEPD2::GDEW0154Z04, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_3C display(GxEPD2::GDEW0213Z16, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_3C display(GxEPD2::GDEW029Z10, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_3C display(GxEPD2::GDEW027C44, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
//GxEPD2_AVR_3C display(GxEPD2::GDEW042Z15, /*CS=*/ EPD_CS, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7);
#endif

// for test on ESP8266
#if defined (ESP8266)
#define SD_CS SS  // e.g. for RobotDyn Wemos D1 mini SD board
#define EPD_CS D1 // alternative I use with RobotDyn Wemos D1 mini SD board
// select one and adapt to your mapping
GxEPD2_AVR_BW display(GxEPD2::GDEP015OC1, /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_BW display(GxEPD2::GDE0213B1, /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_BW display(GxEPD2::GDEH029A1, /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_BW display(GxEPD2::GDEW027W3, /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_BW display(GxEPD2::GDEW042T2, /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_BW display(GxEPD2::GDEW075T8,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
// 3-color e-papers
//GxEPD2_AVR_3C display(GxEPD2::GDEW0154Z04,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_3C display(GxEPD2::GDEW0213Z16,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_3C display(GxEPD2::GDEW029Z10,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_3C display(GxEPD2::GDEW027C44,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_3C display(GxEPD2::GDEW042Z15,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
//GxEPD2_AVR_3C display(GxEPD2::GDEW075Z09,  /*CS=D8*/ EPD_CS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4);
#endif

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  display.init();

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS))
  {
    Serial.println("SD failed!");
    return;
  }
  Serial.println("SD OK!");

  drawBitmapFromSD("parrot.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("betty_1.bmp", 0, 0);
  delay(2000);
#if defined(SdFat_h)
  drawBitmapFromSD("logo200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("first200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("second200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("third200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("fourth200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("fifth200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("sixth200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("senventh200x200.bmp", 0, 0);
  delay(2000);
  drawBitmapFromSD("eighth200x200.bmp", 0, 0);
  delay(2000);
#else
  drawBitmapFromSD("logo.bmp", 0, 0);
  delay(2000);
#endif
}

void loop()
{
}

#define SD_BUFFER_PIXELS 20

void drawBitmapFromSD(const char *filename, uint8_t x, uint8_t y)
{
  FileClass file;
  uint8_t buffer[3 * SD_BUFFER_PIXELS]; // pixel buffer, size for r,g,b
  bool valid = false; // valid format to be handled
  bool flip = true; // bitmap is stored bottom-to-top
  uint32_t pos = 0;
  uint32_t startTime = millis();
  if ((x >= display.width()) || (y >= display.height())) return;
  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');
#if defined(SdFat_h)
  if (!file.open(filename, FILE_READ))
  {
    Serial.print("File not found");
    return;
  }
#else
  file = SD.open(filename);
  if (!file)
  {
    Serial.print("File not found");
    return;
  }
#endif
  // Parse BMP header
  if (read16(file) == 0x4D42) // BMP signature
  {
    uint32_t fileSize = read32(file);
    uint32_t creatorBytes = read32(file);
    uint32_t imageOffset = read32(file); // Start of image data
    uint32_t headerSize = read32(file);
    uint32_t width  = read32(file);
    uint32_t height = read32(file);
    uint16_t planes = read16(file);
    uint16_t depth = read16(file); // bits per pixel
    uint32_t format = read32(file);
    if ((planes == 1) && (format == 0)) // uncompressed is handled
    {
      Serial.print("File size: "); Serial.println(fileSize);
      Serial.print("Image Offset: "); Serial.println(imageOffset);
      Serial.print("Header size: "); Serial.println(headerSize);
      Serial.print("Bit Depth: "); Serial.println(depth);
      Serial.print("Image size: ");
      Serial.print(width);
      Serial.print('x');
      Serial.println(height);
      // BMP rows are padded (if needed) to 4-byte boundary
      uint32_t rowSize = (width * depth / 8 + 3) & ~3;
      if (height < 0)
      {
        height = -height;
        flip = false;
      }
      uint16_t w = width;
      uint16_t h = height;
      if ((x + w - 1) >= display.width())  w = display.width()  - x;
      if ((y + h - 1) >= display.height()) h = display.height() - y;
      if ((depth == 1) && (rowSize < sizeof(buffer))) // handle with direct drawing
      {
        valid = true;
        display.writeScreenBuffer();
        if (flip) file.seek(imageOffset + (height - h) * rowSize);
        else file.seek(imageOffset);
        for (uint16_t row = 0; row < h; row++) // for each line
        {
          file.read(buffer, rowSize);
          uint16_t y = flip ? h - row - 1 : row;
          display.writeImage(buffer, 0, y, w, 1);
        }
        display.refresh();
      }
      else
      {
        display.firstPage();
        do
        {
          size_t buffidx = sizeof(buffer); // force buffer load
          for (uint16_t row = 0; row < h; row++) // for each line
          {
            if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = imageOffset + (height - 1 - row) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = imageOffset + row * rowSize;
            if (file.position() != pos)
            { // Need seek?
              file.seek(pos);
              buffidx = sizeof(buffer); // force buffer reload
            }
            uint8_t bits;
            for (uint16_t col = 0; col < w; col++) // for each pixel
            {
              // Time to read more pixel data?
              if (buffidx >= sizeof(buffer))
              {
                file.read(buffer, sizeof(buffer));
                buffidx = 0; // Set index to beginning
              }
              switch (depth)
              {
                case 1: // one bit per pixel b/w format
                  {
                    valid = true;
                    if (0 == col % 8)
                    {
                      bits = buffer[buffidx++];
                    }
                    uint16_t bw_color = bits & 0x80 ? GxEPD_WHITE : GxEPD_BLACK;
                    display.drawPixel(col, row, bw_color);
                    bits <<= 1;
                  }
                  break;
                case 24: // standard BMP format
                  {
                    valid = true;
                    uint16_t b = buffer[buffidx++];
                    uint16_t g = buffer[buffidx++];
                    uint16_t r = buffer[buffidx++];
                    uint16_t bw_color = ((r + g + b) / 3 > 0xFF  / 2) ? GxEPD_WHITE : GxEPD_BLACK;
                    display.drawPixel(col, row, bw_color);
                  }
                  break;
              }
            } // end pixel
          } // end line
        } // end do
        while (display.nextPage());
      }
    }
  }
  file.close();
  if (valid)
  {
    Serial.print("loaded in ");
    Serial.print(millis() - startTime);
    Serial.println(" ms");
  }
  else
  {
    Serial.println("bitmap format not handled.");
  }
}

uint16_t read16(FileClass& f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(FileClass& f)
{
  // BMP data is stored little-endian, same as Arduino.
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


