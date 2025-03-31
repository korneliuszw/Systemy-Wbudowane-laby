#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#define RS 12
#define E 11
#define D4 7
#define D5 6
#define D6 5
#define D7 4

// location of the left upper corner of the graph
// on the LCD screen
#define GRAPH_POS_X 11
#define GRAPH_POS_Y 0

#define GRAPH_MIN_VAL 0
#define GRAPH_MAX_VAL 1023

#define GRAPH_W_C_COUNT 4 // graph width char count
#define GRAPH_H_C_COUNT 2 // graph height char count
// 2 by 2 chars where each char is 5x8
#define GRAPH_C_WIDTH 5
#define GRAPH_C_HEIGHT 8
#define GRAPH_WIDTH (GRAPH_C_WIDTH * GRAPH_W_C_COUNT)
#define GRAPH_HEIGHT (GRAPH_C_HEIGHT * GRAPH_H_C_COUNT)
uint8_t graph[GRAPH_WIDTH][GRAPH_HEIGHT] = {0};
uint8_t graphChar[GRAPH_H_C_COUNT][GRAPH_W_C_COUNT] = {
  {0, 1, 2, 6},
  {3, 4, 5, 7}
};

int voltageHistory[GRAPH_WIDTH] = {0};
int voltageHistory_it = 0;

#define SWITCHES 2
int switchState[SWITCHES] = {LOW};
int switchMap[] = {2, 8};

// array of times of last debounce for each button
unsigned long debounce[SWITCHES] = {0};
// array of last states for each button, for debouncing
int intermediateStates[SWITCHES] = {0};

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void appendVoltageHistory(int num) {
  voltageHistory[voltageHistory_it] = num;
  voltageHistory_it = (voltageHistory_it + 1) % GRAPH_WIDTH;
}

int getVoltageHistory(int index) {
  return voltageHistory[(voltageHistory_it + index) % GRAPH_WIDTH];
}

void initGraph(){
  // |
  for (int i = 0; i < GRAPH_HEIGHT; i++) {
    graph[0][i] = 1;
  }
  // _
  for (int i = 0; i < GRAPH_WIDTH; i++) {
    graph[i][GRAPH_HEIGHT - 1] = 1;
  }
}

void clearGraph(){
  for (int i = 1; i < GRAPH_WIDTH; i++) {
    for (int j = 0; j < GRAPH_HEIGHT-1; j++) {
      graph[i][j] = 0;
    }
  }
}

void updateGraph(){
  clearGraph();
  for (int i = 0; i < GRAPH_WIDTH; i++) {
    int val = getVoltageHistory(i);
    int height = map(val, GRAPH_MIN_VAL, GRAPH_MAX_VAL, 0, GRAPH_HEIGHT - 1);
    // for loop for fill under the curve
    // single line for just the curve
    // graph[i][GRAPH_HEIGHT -1 - height] = 1;
    for (int j = 0; j < GRAPH_HEIGHT; j++) {
      if (j >= GRAPH_HEIGHT -1 - height) {
        graph[i][j] = 1;
      }
    }
  }
}

void graphToChar(){
  for (int w = 0; w < GRAPH_W_C_COUNT; w++) {
    for (int h = 0; h < GRAPH_H_C_COUNT; h++) {
      int charID = graphChar[h][w];
      byte tempChar[8] = {0};
      for (int cw = 0; cw < GRAPH_C_WIDTH; cw++) {
        for (int ch = 0; ch < GRAPH_C_HEIGHT; ch++) {
          tempChar[ch] |= (graph[w * GRAPH_C_WIDTH + cw][h * GRAPH_C_HEIGHT + ch] << (GRAPH_C_WIDTH - 1 - cw));
        }
      }
      lcd.createChar(charID, tempChar);
    }
  }
}

void printGraphDebugInfo(){
  for (int i = 0; i < GRAPH_WIDTH; i++) {
    int val = getVoltageHistory(i);
    Serial.print(i);
    Serial.print(": ");
    Serial.print(val);
    Serial.print("-");
    Serial.print(map(val, GRAPH_MIN_VAL, GRAPH_MAX_VAL, 0, GRAPH_HEIGHT - 1));
    Serial.print("  ");
  }
  Serial.println();

  for (int i = 0; i < GRAPH_HEIGHT; i++) {
    for (int j = 0; j < GRAPH_WIDTH; j++) {
      Serial.print(graph[j][i]);
    }
    Serial.println();
  }
  Serial.println("------------------");
}

void printGraph(){
  updateGraph();
  graphToChar();

  for (int i = 0; i < GRAPH_W_C_COUNT; i++) {
    for (int j = 0; j < GRAPH_H_C_COUNT; j++) {
      int charID = graphChar[j][i];
      lcd.setCursor(GRAPH_POS_X + i, GRAPH_POS_Y + j);
      lcd.write(charID);
    }
  }
}

void setup() {
  initGraph();
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
  if (decimalPart < 10) {
    lcd.print(" ");
  }
}


void takeVoltage() {
  int read = analogRead(A5);
  appendVoltageHistory(read);
  float voltage = read / 1023.0 * 5.0;
  // lcd.clear();
  lcd.home();
  lcd.print("Voltage: ");
  lcd.setCursor(1, 1);
  printTwoDecimalPlaces(voltage);
  printGraph();
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