//пример использования платы для чайника

#include <OneWire.h> 
#include "pitches.h"
#include <DallasTemperature.h>
#include <LiquidCrystal_1602_RUS.h>
//#include <LiquidCrystal.h>
#include <TimerOne.h>

// пины 
#define RELAY_PIN 7
#define LBUTTON_PIN 2
#define RBUTTON_PIN 4
#define MBUTTON_PIN 3
#define THERMOMETER_PIN 8

//подключение экранчика
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 5, en = 6, d4 = 12, d5 = 11, d6 = 10, d7 = 9;
LiquidCrystal_1602_RUS lcd(rs, en, d4, d5, d6, d7);
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//термодатчик
OneWire tempBus(THERMOMETER_PIN);
DallasTemperature thermometer(&tempBus);
uint8_t sensor_address[8]; 

//temperature regulation
bool reg_active;
float reg_target;
float temp;
bool hold;
bool is_holding;
bool is_ready;
int tackts_left;
bool hold_for_time;
//----------------------

void recipy1();
void recipy2();
void recipy_otl()
{
  beep();
  lcd.setCursor(0, 0);
  lcd.print(L" ПОСТАВЬТЕ 10   ");
  lcd.setCursor(0, 1);
  lcd.print(L"   ПОЖАЛУЙСТА)  ");
  while(digitalRead(MBUTTON_PIN)){}
  beep();
}

//recipy selector
struct recipy
{
  wchar_t * name;
  void (*func)();
};
#define RECIPY_NUM 3
recipy rec_arr[RECIPY_NUM] = {
  {L"   ГЛИНТВЕЙН    ", recipy1},
  {L"      ПУНШ      ", recipy2},
  {L"     ОТЛ10:)    ", recipy_otl}
  };
//---------------

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(7, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(7);
  }
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LBUTTON_PIN, INPUT_PULLUP);
  pinMode(MBUTTON_PIN, INPUT_PULLUP);
  pinMode(RBUTTON_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);
  
  thermometer.begin();
  thermometer.getAddress(sensor_address, 0);
  //thermometer.setResolution(9);
  
  reg_init();
}

void loop() {
  recipy_selector();
}




void recipy1()
{
  beep();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PUT FRUITES &");
  lcd.setCursor(0, 1);
  lcd.print("SPICE PRESS OK"); 
  
  while(digitalRead(MBUTTON_PIN)){}
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("WAIT");
  set_reg(95, 1, 900);
  
  reg_waiting_loop();

  beep();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PURE WINE");
  lcd.setCursor(0, 1);
  lcd.print("PRESS OK"); 

  while(digitalRead(MBUTTON_PIN)){}
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("WAIT"); 
  set_reg(75, 1, 300);
  
  reg_waiting_loop();

  beep();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("READY!"); 
  lcd.setCursor(0, 1);
  lcd.print("HOLDIND TEMP");
  
  while(digitalRead(MBUTTON_PIN)){}
  stop_reg();
}


void recipy2(){
  beep();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PUT 0,5 L TEA & ");
  lcd.setCursor(0, 1);
  lcd.print("SPICE PRESS OK"); 
  
  while(digitalRead(MBUTTON_PIN)){}
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("WAIT 30 MIN");
  set_reg(95, 0);
  reg_waiting_loop();
  set_reg(60, 1, 1800);
  reg_waiting_loop();

  beep();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PUT 0,2 L WINE");
  lcd.setCursor(0, 1);
  lcd.print("PRESS OK"); 

  while(digitalRead(MBUTTON_PIN)){}
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("WAIT"); 
  set_reg(75, 1, 300);
  
  reg_waiting_loop();

  beep();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("READY! ADD VODKA!"); 
  lcd.setCursor(0, 1);
  lcd.print("HOLDIND TEMP");
  
  while(digitalRead(MBUTTON_PIN)){}
  stop_reg();
}








void recipy_selector()
{
  bool lb = digitalRead(LBUTTON_PIN);
  bool rb = digitalRead(RBUTTON_PIN);
  bool mb = digitalRead(MBUTTON_PIN);

  lcd.setCursor(0, 1);
  lcd.print("<      OK      >");
  
  
  int i = 0;
  while(1)
  {
    lcd.setCursor(0, 0);
    lcd.print(rec_arr[i].name);
    if(lb && !digitalRead(LBUTTON_PIN)) i--;
    if(rb && !digitalRead(RBUTTON_PIN)) i++;
    if(mb && !digitalRead(MBUTTON_PIN))
    {
      rec_arr[i].func();
      break;
    }
    lb = digitalRead(LBUTTON_PIN);
    rb = digitalRead(RBUTTON_PIN);
    mb = digitalRead(MBUTTON_PIN);
    if(i < 0) i = RECIPY_NUM - 1;
    if(i == RECIPY_NUM) i = 0;
    delay(50);
  }
}



void temp_reg()
{ 
  if(reg_active)
  { 
    temp = read_temp();
    if(hold || (!hold && !is_ready))
    {
      if(temp < reg_target)
      {
        digitalWrite(RELAY_PIN, 1);
      }
      else
      {
        if(!hold || tackts_left < 0) is_ready = true;
        is_holding = hold;
        digitalWrite(RELAY_PIN, 0);
      }
      if(is_holding && tackts_left >= 0) tackts_left --; 
    }
    else digitalWrite(RELAY_PIN, 0);
  }
  else digitalWrite(RELAY_PIN, 0);
}

void reg_waiting_loop()
{
  while(!is_reg_ready())
  {
    cli();
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    if(!is_ready && !is_holding) lcd.print("HEAT ");
    else if(is_holding && !is_ready) lcd.print("HOLD ");
    else lcd.print("TEMP ");
    lcd.print(temp);
    lcd.print('/');
    lcd.print(reg_target);
    sei();
    delay(100);
  }
}

void set_reg(float t, int mode, int time)
{
  hold_for_time = 0;
  tackts_left = time * 2;
  if(mode == 0) hold = 0;
  if(mode == 1) hold = 1;
  reg_target = t;
  is_ready = 0;
  is_holding = 0;
  reg_active = 1;
}

void set_reg(float t, int mode)
{
  hold_for_time = 0;
  tackts_left = -1;
  if(mode == 0) hold = 0;
  if(mode == 1) hold = 1;
  reg_target = t;
  is_ready = 0;
  is_holding = 0;
  reg_active = 1;
}

void stop_reg()
{
  reg_active = 0;
}

bool is_reg_ready()
{
  return is_ready;
}

void reg_init()
{
  Timer1.initialize(500000);
  Timer1.attachInterrupt(temp_reg);
  reg_active = 0;
}

float read_temp()
{
  thermometer.requestTemperaturesByAddress(sensor_address);
  return thermometer.getTempC(sensor_address);
}

void beep()
{
  for(int i = 0; i < 60; i++)
  {
    digitalWrite(RELAY_PIN, 1);
    delay(2);
    digitalWrite(RELAY_PIN, 0);
    delay(2);
  }
}




 
