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

char ludzik[] = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
  0b00000,
};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2, LCD_5x8DOTS);
  pinMode(MOTION, INPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  lcd.createChar(0, ludzik);
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
  delayMicroseconds(20);
  digitalWrite(TRIG, LOW);
  unsigned long echoDuration = pulseIn(ECHO, HIGH);
  return echoDuration / 58.0;
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

void showProgress(double distance, bool motion) {
  static int cursor = 0;
  if (distance < 150.0) {
    lcd.setCursor(cursor, 0);
    lcd.print(" ");
    cursor = distance / 10;
  }
  lcd.setCursor(cursor, 0);
  lcd.write('\0');
}

void loop() {
  bool motion = shouldPrintMotionDetected();
  double distance = measureDistanceCm();
  Serial.println(distance, DEC);
  distance = measureDistanceCm();
  showProgress(distance, motion);
}