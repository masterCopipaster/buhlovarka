//пример использования платы для чайника

#include <LiquidCrystal.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

// пины 
#define RELAY_PIN 7
#define BUTTON_PIN 2
#define THERMOMETER_PIN 8

//подключение экранчика
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 5, en = 6, d4 = 12, d5 = 11, d6 = 10, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//термодатчик
OneWire tempBus(THERMOMETER_PIN);
DallasTemperature thermometer(&tempBus);

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin(16, 2);
  thermometer.begin();
}

void loop() {
  lcd.setCursor(0, 0);
  thermometer.requestTemperatures();

  float t = thermometer.getTempCByIndex(0);
  lcd.print(t);

  digitalWrite(RELAY_PIN, t < 70);
  
  lcd.setCursor(0, 1);
  //чтение кнопки
  if(!digitalRead(BUTTON_PIN)) lcd.print("button pressed"); // нажатая кнопка подтягивает вход к земле
  else lcd.print("                 "); // когда кнопка отпущена вход подтянут к 1
  delay(300);
}
 
