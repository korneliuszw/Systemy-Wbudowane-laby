#ifndef _LCD_H
#define _LCD_H

#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>

#define LCD_WIDTH ILI9341_TFTWIDTH
#define LCD_HEIGHT ILI9341_TFTHEIGHT

class LCD : public Adafruit_GFX {
private:
    Adafruit_ILI9341 lcd;

public:
    // rst = 8 in schema
    LCD(int8_t dc = 7, int8_t rst = -1, int8_t bl = 9,
        int8_t cs = 10, int8_t din = 11, int8_t clk = 13,
        uint16_t screenWidth = LCD_WIDTH,
        uint16_t screenHeight = LCD_HEIGHT);

    void begin();
    void clear(uint16_t color = ILI9341_BLACK);
    void reset();

    void fillScreen(uint16_t color) override {
        lcd.fillScreen(color);
    };

    void setRotation(uint8_t r) override {
        lcd.setRotation(r);
    };

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        lcd.drawPixel(x, y, color);
    }
};

#endif // _LCD_H