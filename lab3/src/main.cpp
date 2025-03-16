#include <Arduino.h>
#include <LiquidCrystal.h>

#define RS 12
#define E 11
#define D4 5
#define D5 4
#define D6 3
#define D7 2

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void setup() {
    lcd.begin(16, 2, LCD_5x8DOTS);
    lcd.print("Hello World!");
}

void loop() {
}