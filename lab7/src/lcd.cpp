#include <lcd.h>

// TODO: Handle bl pin
// din = mosi, clk = sck
LCD::LCD(int8_t dc, int8_t rst, int8_t bl, int8_t cs, int8_t din, int8_t clk,
         uint16_t screenWidth, uint16_t screenHeight):
    Adafruit_GFX(screenWidth, screenHeight),
    lcd(cs, dc, din, clk, rst){};

void LCD::begin(){
    lcd.begin();
    // setRotation(1); // Horizontal mode is 1 or 3
    // setTextSize(2);
}

void LCD::clear(uint16_t color){
    lcd.fillScreen(color);
}

void LCD::reset(){
    lcd.fillScreen(ILI9341_BLACK);
    setTextSize(2);
    setCursor(0, 0);
    setTextColor(ILI9341_WHITE);
}