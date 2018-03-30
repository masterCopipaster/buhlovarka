//пример использования платы для чайника
//на данный момент контроллер не хочет запускаться от кристалла
//поэтому тактовая частота не 16МГц а 8МГц
//поэтому все времена надо подгонять
//кристалл пофикшу

#include <LiquidCrystal.h>

// пины 
#define RELAY_PIN 7
#define BUTTON_PIN 2
#define THERMOMETER_PIN 8

//подключение экранчика
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 5, en = 6, d4 = 12, d5 = 11, d6 = 10, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.begin(16, 2);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("tick: ");
  lcd.print(millis() / 500); //откорректированное время (вместо 1000 стоит 500)
  //на четных реле выкл, на нечетных вкл
  digitalWrite(7, (millis() / 500) & 1);
  
  lcd.setCursor(0, 1);
  //чтение кнопки
  if(!digitalRead(2)) lcd.print("button pressed"); // нажатая кнопка подтягивает вход к земле
  else lcd.print("                 "); // когда кнопка отпущена вход подтянут к 1
}

