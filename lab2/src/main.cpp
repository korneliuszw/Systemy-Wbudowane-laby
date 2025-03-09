#include <Arduino.h>

#define SEGMENT_A 2
#define SEGMENT_B 3
#define SEGMENT_C 4
#define SEGMENT_D 5
#define SEGMENT_E 6
#define SEGMENT_F 7
#define SEGMENT_G 8
#define SEGMENT_DOT 9

// Columns
#define P1 A1 // 15
#define P2 A2
#define P3 A3
#define P4 A4 // 18
short int columns[] = {P4, P3, P2, P1};
// Rows
#define P5 11
#define P6 12
#define P7 13
#define P8 A0 // 14
short int rows[] = {P8, P7, P6, P5};

#define BTN_CLEAR '#'
#define BTN_DEL '*'

// -2 = don't care
char keypad[4][4] = {
  {1, 2, 3, -2},
  {4, 5, 6, -2},
  {7, 8, 9, -2},
  {BTN_DEL, 0, BTN_CLEAR, -2}
};

char readKeyPad() {
  char val = -1;
  // for(int i = 0; i < 4; i++) {
  //   digitalWrite(columns[i], HIGH);
  // }
  for(int i = 0; i < 4; i++) {
    digitalWrite(columns[i], LOW);
    for(int j = 0; j < 4; j++) {
      if (digitalRead(rows[j]) == LOW) {
        val = keypad[j][i];
      }
    }
    digitalWrite(columns[i], HIGH);
  }
  return val;
}

#define DIGITS 4
unsigned char digits[] = {2, 1, 3, 7};

inline void writeSegment(unsigned char segment, unsigned char value) {
  digitalWrite(segment, (value & 1) == 1 ? LOW : HIGH);
}

// 0..9 - digits, 10..13 - A,B,C,D
void writeToLcd(unsigned int in) {
  unsigned char d = in & 1, c = (in >> 1) & 1, b = (in >> 2) & 1, a = (in >> 3) & 1;
  digitalWrite(SEGMENT_DOT, HIGH);
  // inputs are solved karnaugh maps for this int
  writeSegment(SEGMENT_A, c | a | (~b & ~d) | (b&d)); // C + A + B'D' + BD
  writeSegment(SEGMENT_B, ~b | (c&d) | (a&d) | (~a & ~c & ~d)); // B' + CD + AD + A'C'D'
  writeSegment(SEGMENT_C, ((~b & ~c) | (~a & d) | (~a & b) | (a & ~b & ~d))); //  B'C' + A'D + A'B + AB'D'
  writeSegment(SEGMENT_D, ((a & ~c) | (~b & c & d) | (~a & ~b & ~d) | (~a & c & ~d) | (b & ~c & d) )); // AC' + B'CD + A'B'D' + A'CD' + BC'D
  writeSegment(SEGMENT_E, ((~b & ~d) | (c & ~d) | (a & c) | (a & b))); //  B'D' + CD' + AC + AB
  writeSegment(SEGMENT_F, (a | (~c & ~d) | (b & ~c) | (b & ~d) )); //  A + C'D' + BC' + BD'
  writeSegment(SEGMENT_G, ((~b & c) | (a & ~b) | (c & ~d) | (~a & b & ~c))); // B'C + AB' + CD' + A'BC'
}

void clearSegments() {
  for (int i = SEGMENT_A; i <= SEGMENT_G; i++) {
    digitalWrite(i, HIGH);
  }
}

void printDigit(unsigned char display, char in) { 
  for (int i = 15; i <= 18; i++) {
    digitalWrite(i, LOW);
  }
  if (display > 3) return;
  int pin = 15 + display;
  if (in >= 0) {
    writeToLcd(in);
  } else {
    clearSegments();
  }
  digitalWrite(pin, HIGH);
  delay(2);
  digitalWrite(pin, LOW);
  clearSegments();
}

void setup() {
  Serial.begin(9600);
  for (int i = 2; i <= 10; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i = 15; i <= 18; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i = 11; i <= 14; i++) {
    pinMode(i, INPUT_PULLUP);
  }
}


unsigned int currentNumber = 0;

void readDigitValue() {
  static char previous_key = -1;
  char key = readKeyPad();
  if (previous_key == key){

  } else if (key == BTN_CLEAR) {
    currentNumber = 0;
  }else if (key == BTN_DEL) {
    currentNumber /= 10;
  } else  if (previous_key == -1 && key != previous_key && currentNumber <= 999) {
    currentNumber = (currentNumber * 10) + key;
  }
  previous_key = key;
  if (key < 0) return;
  for (int i = 0; i < DIGITS; i++) {
    digitalWrite(columns[i], LOW);
  }
}

void loop() {
  readDigitValue();
  int number = currentNumber;
  char digit = number % 10;
  for (int i = 0; i < DIGITS; i++) {
    printDigit(i, digit);
    if (number < 10) digit = -1;
    else {
      number /= 10;
      digit = number % 10;
    }
  }
  Serial.println();
}