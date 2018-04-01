//пример использования платы для чайника

#include <OneWire.h> 
#include <DallasTemperature.h>
//#include <LiquidCrystal_1602_RUS.h>
#include <LiquidCrystal.h>
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
//LiquidCrystal_1602_RUS lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


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

void setup() {
  
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
  recipy1();
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
  for(int i = 0; i < 100; i++)
  {
    digitalWrite(RELAY_PIN, 1);
    delay(2);
    digitalWrite(RELAY_PIN, 0);
    delay(2);
  }
}




 
