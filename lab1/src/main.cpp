#include <Arduino.h>

#define SWITCHES 5
#define SWITCH_OFFSET 2

int switchState[SWITCHES] = {LOW};
int switchMap[] = {12, 13, 11, 10, 40};
char rgbState = 0;

// array of times of last debounce for each button
unsigned long debounce[SWITCHES] = {0};
// array of last states for each button, for debouncing
int intermediateStates[SWITCHES] = {0};

void changeRgb() {
  // 9 - R 8 - B 7 - G
  bool rState = rgbState & 1 ? HIGH : LOW;
  bool gState = (rgbState >> 1) & 1 ? HIGH : LOW;
  bool bState = (rgbState >> 2) & 1 ? HIGH : LOW;
  digitalWrite(9, rState);
  digitalWrite(8, gState);
  digitalWrite(7, bState);
  rgbState = (rgbState + 1) % 8;

}

void changeLed(int led, int buttonVal) {
  digitalWrite(led, buttonVal);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < SWITCHES; i++) {
    pinMode(SWITCH_OFFSET + i, INPUT_PULLUP);
  }
  // last is rgb so ignore it
  for (int i = 0; i < SWITCHES - 1; i++) {
    pinMode(switchMap[i], OUTPUT);
    changeLed(switchMap[i], HIGH);
    delay(500);
    changeLed(switchMap[i], LOW);
    delay(50);
  }
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  for (int i = 0; i < 8; i++) {
    changeRgb();
    delay(200);
  }
}


void loop() {
  unsigned long ms = millis();
  for (int i = 0; i < SWITCHES; i++) {
    int val = digitalRead(SWITCH_OFFSET + i);
    if (val != intermediateStates[i]) {
      debounce[i] = ms;
    }
    else if ((ms - debounce[i]) > 50 && val != switchState[i]) {
      switchState[i] = val;
      if (switchMap[i] == 40) {
        Serial.println("Changed state of RGB");
        if (val == HIGH)
          changeRgb();
      } else if (val == HIGH) {
        Serial.print("Changed state of ");
        Serial.print(i, DEC);
        Serial.print(" to ");
        Serial.println(val, DEC);
        changeLed(switchMap[i], digitalRead(switchMap[i]) == HIGH ? LOW : HIGH);
      }
    }
    intermediateStates[i] = val;
  }
}
