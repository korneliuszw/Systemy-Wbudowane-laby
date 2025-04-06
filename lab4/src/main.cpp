#include <Arduino.h>
#include <LiquidCrystal.h>
#include <i2c_BMP280.h>
#define RS 12
#define E 11
#define D0 9
#define D4 5
#define D5 4
#define D6 3
#define D7 2


LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
BMP280 bmp;


enum CurrentUnit {
  CELCIUS,
  FAHRENHEIT,
  KELVIN
};

CurrentUnit units = CELCIUS;

float getConvertedUnit(float temp) {
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
      lcd.print((char) 223);
      lcd.print("C");
      break;
    case FAHRENHEIT:
      lcd.print((char) 223);
      lcd.print("F");
      break;
    case KELVIN:
      lcd.print("K");
      break;
  }
}

void printTemperature(float temp) {
  lcd.print("T: ");
  Serial.print("T=");
  Serial.println(temp, DEC);
  // int intPart = (int) temp;
  // float twoDecimalsPlaces = (temp - intPart) * 100.0;
  lcd.print(temp, DEC);
  // if (twoDecimalsPlaces > 0) {
  //   lcd.print(".");
  //   lcd.print((int) twoDecimalsPlaces,  DEC);
  // }
  printUnit();
}
void printPressure(float pascals) {
  lcd.print("P:");
  // Serial.print("P=");
  // Serial.println(press, DEC);
  // int intPart = (int) pascals;
  // float twoDecimalsPlaces = (pascals - intPart) * 100.0;
  lcd.print(pascals, DEC);
  // if (twoDecimalsPlaces > 0) {
  //   lcd.print(".");
  //   lcd.print((int) twoDecimalsPlaces,  DEC);
  // }
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
    bmp.initialize();
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.println("Hello world!");
    // lcd.print("Hello World!");
    // lcd.cursor();
}

void loop() {
  // return;
  checkSwitches();
  // returns time to wait for measurement 
  bmp.awaitMeasurement();
  Serial.println("Gow measuremenet");
  float temp, pres;
  // 0 if failed
  bmp.getTemperature(temp);
  bmp.getPressure(pres);
  Serial.print(temp, DEC);
  Serial.print(" ");
  Serial.println(pres, DEC);
  lcd.clear();
  printTemperature(temp);
  lcd.setCursor(0, 1);
  printPressure(pres);
  delay(2);
}