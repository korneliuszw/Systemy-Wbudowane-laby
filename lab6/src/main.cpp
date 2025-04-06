#include <Arduino.h>
#include <LiquidCrystal.h>
#define RS 12
#define E 11
#define D4 5
#define D5 4
#define D6 3
#define D7 2

#define ECHO 9
#define TRIG 8
#define MOTION 10
#define MOTION_DURATION 500

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2, LCD_5x8DOTS);
  pinMode(MOTION, INPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
}

void printTwoDecimalPlaces(double num) {
  int intPart = (int) num;
  int decimalPart = (num - (double) intPart) * 100;
  lcd.print(intPart);
  lcd.print(".");
  lcd.print(decimalPart);
}


double measureDistanceCm() {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long echoDuration = pulseIn(ECHO, HIGH);
  return echoDuration / 2.0 / 29.1;
}

inline bool didDetectMotion() {
  int pirState = digitalRead(MOTION);
  return pirState == HIGH;
}

bool shouldPrintMotionDetected() {
  static unsigned long lastMotionDetectedTime = 0;
  auto time = millis();
  if (didDetectMotion()) {
    lastMotionDetectedTime = time;
  }
  return time < lastMotionDetectedTime + MOTION_DURATION;
}

void loop() {
  lcd.clear();
  if (shouldPrintMotionDetected()) {
    lcd.print("Ruch");
  } else {
    lcd.print("Brak ruchu");
  }
  double distance = measureDistanceCm();
  lcd.setCursor(0,1);
  lcd.print("Odl: ");
  if (distance > 200.0) {
    lcd.print("OoR");
  } else {
    printTwoDecimalPlaces(distance);
  }
  delay(100);
}