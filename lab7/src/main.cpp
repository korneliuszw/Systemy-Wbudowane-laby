#include <Arduino.h>

#include <Adafruit_MPU6050.h> // Accelerometer
#include "Adafruit_ILI9341.h" // LCD

#include <lcd.h>
#include <accelerometer.h>
#include <orientation.h>

// Accelerometer
// Adafruit_MPU6050 mpu;
Accelerometer acc;

// LCD
#define TFT_DC 7
#define TFT_CS 10
Adafruit_ILI9341 lcd = Adafruit_ILI9341(TFT_CS, TFT_DC);
// LCD lcd = LCD();


void initLcd(){
  lcd.begin();
  lcd.setRotation(1); // Horizontal mode is 1 or 3
}

void resetLcd(){
  lcd.fillScreen(ILI9341_BLACK);
  lcd.setCursor(0, 0);
  lcd.setTextColor(ILI9341_WHITE);
  lcd.setTextSize(2);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause until serial console opens

  Serial.println("Starting...");

  acc.begin();
  initLcd();

  Serial.println("Setup finished");
}

void loop() {

  sensors_event_t a;
  acc.getEvent(&a);

  resetLcd();
  // lcd.reset();

  lcd.println("Acceleration:");
  lcd.print("X: ");
  lcd.println(a.acceleration.x);
  lcd.print("Y: ");
  lcd.println(a.acceleration.y);
  lcd.print("Z: ");
  lcd.println(a.acceleration.z);
  lcd.println("m/s^2");
  lcd.println();

  Orientation o = acc.getOrientation();
  lcd.println("Orientation:");
  lcd.print("Pitch: ");
  lcd.println(o.pitch);
  lcd.print("Roll:  ");
  lcd.println(o.roll);
  lcd.print("Yaw:   ");
  lcd.println(o.yaw);
  lcd.println("degrees");
  lcd.println();


  lcd.println("");
  delay(1000);
}