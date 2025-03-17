#include <Arduino.h>
#include <LiquidCrystal.h>
// #include <LiquidCrystal_I2C.h>

#define RS 12
#define E 11
#define D0 9
#define D1 8
#define D2 7
#define D3 6
#define D4 5
#define D5 4
#define D6 3
#define D7 2

#define FRAME_CHARS 8

// 8 znakow 5x8 pixeli
// wyswietlacz 2x16 znakow
// wiec mozna podzielic na 2x 8 znakow czyli potrzeba zdefiniowac 16 znakow?

constexpr int FRAME_SIZE = 8*8;
byte buffer[FRAME_SIZE] = {0};
LiquidCrystal lcd(RS, E, D0, D1, D2, D3, D4, D5, D6, D7);
// LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2, LCD_5x8DOTS);
    // lcd.print("Hello World!");
    // lcd.cursor();
  
}

int frameCounter = 0;

void readFrame() {
    auto rx = Serial.readBytes(buffer, 8 * 8);
    Serial.println("Received smh");
    Serial.println(rx, DEC);
    if (rx < FRAME_SIZE) {
        // what to do now?
        return;
    }
    lcd.clear();
    for (byte ch = 0; ch < 8; ch++) {
        lcd.createChar(ch, buffer+(ch*8));
    }
    byte ch = 0;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 4; x++) {
            lcd.setCursor(x, y);
            lcd.write(ch++);
        }
    }
    frameCounter++;
    Serial.print("Frame: ");
    Serial.println(frameCounter, DEC);
    Serial.write(0xDEADBEEF);
}

void loop() {
    // // lcd.noCursor();
    // // lcd.noBlink();
    // // lcd.noAutoscroll();
    // lcd.clear(); 
//     byte           deg[8] =            //creating a special LCD character for the "degree" symbol
// {
//   B01111,
//   B01111,
//   B01110,
//   B00000,
//   B01110,
//   B00000,
//   B00000,
//   B00000
// };
//     lcd.createChar(0, deg);
    if (Serial.available()) {
        readFrame();
        delay(2);
        Serial.write(byte(245));
    }
}