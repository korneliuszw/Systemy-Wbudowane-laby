#include <Arduino.h>
#include <LiquidCrystal.h>
#include <BMP280.h>
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


LiquidCrystal lcd(RS, E, D0, D1, D2, D3, D4, D5, D6, D7);
BMP280 bmp;


enum CurrentUnit {
  CELCIUS,
  FAHRENHEIT,
  KELVIN
};

CurrentUnit units = CELCIUS;

double getConvertedUnit(double temp) {
  switch (units) {
    case CELCIUS:
      return temp;
    case FAHRENHEIT:
      return 1.8 * temp + 32;
    case KELVIN:
      return temp + 273.15;
  }
}

void printUnit() {
  switch (units) {
    case CELCIUS:
      lcd.print(223);
      lcd.print("C");
      break;
    case FAHRENHEIT:
      lcd.print(223);
      lcd.print("F");
      break;
    case KELVIN:
      lcd.print("K");
      break;
  }
}

void printTemperature(double temp) {
  lcd.print("T: ");
  Serial.print("T=");
  Serial.println(temp, DEC);
  int intPart = (int) temp;
  int twoDecimalsPlaces = (temp - intPart) * 100.0;
  lcd.print(intPart, DEC);
  if (twoDecimalsPlaces > 0) {
    lcd.print(".");
    lcd.print(twoDecimalsPlaces,  DEC);
  }
  printUnit();
  lcd.println();
}
void printPressure(double press) {
  double pascals = press / 100000;
  lcd.print("P:");
  Serial.print("P=");
  Serial.println(press, DEC);
  int intPart = (int) pascals;
  int twoDecimalsPlaces = (pascals - intPart) * 100.0;
  lcd.print(intPart, DEC);
  if (twoDecimalsPlaces > 0) {
    lcd.print(".");
    lcd.print(twoDecimalsPlaces,  DEC);
  }
  lcd.println("Pa");
}

void checkSwitches() {
  if (Serial.available() == 0) return;
  while (Serial.available() > 0) {
    String str = Serial.readString();
    if (str.equalsIgnoreCase("fahrenheit")) {
      units = FAHRENHEIT;
    } else if (str.equalsIgnoreCase("celcius")) {
      units = CELCIUS;
    } else if (str.equalsIgnoreCase("kelvin")) {
      units = KELVIN;
    }
  }
}

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2, LCD_5x8DOTS);
    // lcd.print("Hello World!");
    // lcd.cursor();
    bmp.setOversampling(4);
}

void loop() {
  checkSwitches();
  // returns time to wait for measurement 
  char result = bmp.startMeasurment();
  if (result == 0) {
    Serial.println("Failed to start measure"); 
    return;
  }
  delay(result);
  double temp, pres;
  // 0 if failed
  result = bmp.getTemperatureAndPressure(temp, pres);
  if (result == 0) {
    Serial.println("Fail to receive measurement");
    return;
  }
  lcd.clear();
  printTemperature(temp);
  printPressure(pres);
  delay(2);
}