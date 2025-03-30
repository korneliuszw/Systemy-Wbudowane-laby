#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#define RS 12
#define E 11
#define D4 7
#define D5 6
#define D6 5
#define D7 4

#define SWITCHES 2
int switchState[SWITCHES] = {LOW};
int switchMap[] = {2, 8};

// array of times of last debounce for each button
unsigned long debounce[SWITCHES] = {0};
// array of last states for each button, for debouncing
int intermediateStates[SWITCHES] = {0};

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2, LCD_5x8DOTS);
  for (int i = 0; i < SWITCHES; i++) {
    pinMode(switchMap[i], INPUT_PULLUP);
  }
  // every 0.5s
  Timer1.initialize(500000UL);
}

void printTwoDecimalPlaces(float num) {
  int intPart = (int) num;
  int decimalPart = (num - (float) intPart) * 100;
  lcd.print(intPart);
  lcd.print(".");
  lcd.print(decimalPart);
}

void takeVoltage() {
  int read = analogRead(A5);
  float voltage = read / 1023.0 * 5.0;
  lcd.clear();
  lcd.print("Voltage: ");
  printTwoDecimalPlaces(voltage);
}

void switchMode() {
  static bool autoRead = false;
  if (autoRead) {
    Timer1.attachInterrupt(takeVoltage);
  } else {
    Timer1.detachInterrupt();
    takeVoltage();
  }
  autoRead = !autoRead;
}


void loop() {
  unsigned long ms = millis();
  for (int i = 0; i < SWITCHES; i++) {
    int val = digitalRead(switchMap[i]);
    if (val != intermediateStates[i]) {
      debounce[i] = ms;
    }
    else if ((ms - debounce[i]) > 50 && val != switchState[i]) {
      switchState[i] = val;
      if (val == HIGH) {
        if (i == 0) takeVoltage();
        else switchMode();
      }
    }
    intermediateStates[i] = val;
  }
}