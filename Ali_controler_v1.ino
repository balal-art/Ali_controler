//#include <Arduino.h>
//#include <PID_v1.h>

//------------------------------- library  ---------------------------------//
// define max number of tasks to save precious Arduino RAM
#include <Streaming.h>
#include <Array.h>
#define TASKER_MAX_TASKS 4
#include "myTypes.h"
#include "config.h"
#include "myRelay.h"
#include "myButton.h"
//#include <Smoothed.h>
#include <ACS712.h>
#include <RTClib.h>
//#include <SerialCommands.h> https://github.com/ppedro74/Arduino-SerialCommands
#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//menu
#include <menu.h> //menu macros and objects
#ifdef LCD216
#include <menuIO/liquidCrystalOut.h>
#endif
#ifdef GLCD12864
#include <menuIO/u8g2Out.h>
#define fontName u8g2_font_6x10_tr //u8g2_font_5x7_tf
#define fontX 6                    //5
#define fontY 12                   //9
#define offsetX 0
#define offsetY 0
#define U8_Width 128
#define U8_Height 64

#endif

#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

using namespace Menu;
//-------------------------------- define ----------------------------------//

#ifdef GLCD12864
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// this is a monochromatic color table
const colorDef<uint8_t> colors[6] MEMMODE = {
  {{0, 0}, {0, 1, 1}}, //bgColor
  {{1, 1}, {1, 0, 0}}, //fgColor
  {{1, 1}, {1, 0, 0}}, //valColor
  {{1, 1}, {1, 0, 0}}, //unitColor
  {{0, 1}, {0, 0, 1}}, //cursorColor
  {{1, 1}, {1, 0, 0}}, //titleColor
};
#endif
//------------------------------- VARIABLE ---------------------------------//
//test--------------------------

//clock-------------------------
DateTime clock_now;
byte clockHours = 0;
byte clockMinutes = 0;
int clockYear = 2020;
byte clockMonth = 02;
byte clockDay = 02;
//timer-------------------------
unsigned long time_last = 0;
unsigned long time_cunter = 0;
unsigned long time_lcd_last = 0;
unsigned long time_ds18b20_last = 0;
unsigned long time_battery_last = 0;
//voltage & Current-------------
float battery_current = 0.00f;
float battery_power = 0;
float battery_voltage = 0.0f;
int battery_level = 0;

//display-----------------------
byte page_state = 0;
#define PAGE_NUMBER 3
//loger-------------------------
int8_t log_temp[(SWHO_LOG_COUNT)] = {SWHO_LOG_ARRAY, SWHO_LOG_ARRAY, SWHO_LOG_ARRAY, SWHO_LOG_ARRAY, SWHO_LOG_ARRAY, SWHO_LOG_ARRAY};
Array<int8_t> log_temperature = Array<int8_t>(log_temp, SWHO_LOG_COUNT);
//Temperature-------------------
double temperature_1_now = 21.00;
double temperature_1_min = 10000.00;
double temperature_1_max = 21.00;
double temperature_1_offset = 0;

double temperature_curent = 21;

//Boiler------------------------
myRelay Boiler(BOILER_PIN);

int _pin = 9; //BOILER_PIN;
double boiler_temp_target = 24;
double boiler_temp_Hysteresis = 1.2;
/*
  enum Out_State boiler_state = OUT_OFF;
  int boiler_pwm_on = 50; //12volt=>100% 6volt=>50%
  int boiler_delay_on = 2000;
  int boiler_pwm_max_off = 50;
  int boiler_pwm_min_off = 30;
  int boiler_delay_off = 5000;
  unsigned long boiler_time_on = 0;
  unsigned long boiler_time_off = 0;
  int boiler_pwm_out = 0;
*/
//--------------------------- opject define  -------------------------------//
//DS1307----------------------
RTC_DS1307 rtc;
//oneWire---------------------
OneWire oneWire_DS18B20(DS18B20_PIN);        // Setup oneWire
DallasTemperature ds18b20(&oneWire_DS18B20); //  oneWire to Dallas Temperature.
#define ds18b20_count
const DeviceAddress ds18b20_address[2] = {
  {0x28, 0xAA, 0xF5, 0x96, 0x3B, 0x14, 0x01, 0x87}, {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0}
};
//cmd ------------------------------------------------------------------//
/*
  char serial_command_buffer_[32];
  SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
  //This is the default handler, and gets called when no other command matches.
  void cmd_unrecognized(SerialCommands* sender, const char* cmd)
  {
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
  }
*/
//Lcd shild 2*16------------------------------------------------------------//
#ifdef LCD216
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif
//GLcd 128 64---------------------------------------------------------------//
#ifdef GLCD12864
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R2, GlCD_E, GLCD_RW, GLCD_RS, GLCD_RST);
#endif
//--------------------------------- menu  ----------------------------------//
#define MAX_DEPTH 4
serialIn serial(Serial);

//a menu using a plugin field
MENU(ds18b20_menu, "Ds18b20", doNothing, anyEvent, noStyle, EXIT("<Back"), FIELD(temperature_1_offset, "offset", "s", -10, 10, 1, 0.05, doNothing, noEvent, noStyle));

MENU(boiler_menu, "Boiler", doNothing, anyEvent, noStyle, EXIT("<Back"), FIELD(boiler_temp_target, "Temp Off ", "C", 0, 50, 1, 0.1, doNothing, noEvent, noStyle), FIELD(boiler_temp_Hysteresis, "Temp On", "C", 0, 10, 1, 0.1, doNothing, noEvent, noStyle), FIELD(Boiler.pwm_on_min, "pwmON", "~", 0, 255, 10, 1, doNothing, noEvent, noStyle), FIELD(Boiler.on_delay, "delayON", "ms", 0, 5000, 100, 10, doNothing, noEvent, noStyle), FIELD(Boiler.pwm_off_max, "pwmMaxOFF", "~", 0, 255, 10, 1, doNothing, noEvent, noStyle), FIELD(Boiler.pwm_off_min, "pwmMinOFF", "~", 0, 255, 10, 1, doNothing, noEvent, noStyle), FIELD(Boiler.off_delay, "delayOFF=", "ms", 0, 10000, 100, 10, doNothing, noEvent, noStyle), FIELD(Boiler.off_repet, "repetOFF=", "n", 0, 15, 1, 0, doNothing, noEvent, noStyle));

MENU(setting_menu, "Setting", doNothing, anyEvent, noStyle, EXIT("<Back"), SUBMENU(ds18b20_menu), SUBMENU(boiler_menu));
//clock menu---------------------------------------

result updateClockMenu(eventMask e)
{
  clock_now = rtc.now();
  clockHours = clock_now.hour();
  clockMinutes = clock_now.minute();
  clockYear = clock_now.year();
  clockMonth = clock_now.month();
  clockDay = clock_now.day();
  return proceed;
}

result saveDate(eventMask e)
{
  rtc.adjust(DateTime(clockYear, clockMonth, clockDay, clock_now.hour(), clock_now.minute(), 0));
  return proceed;
}

result saveTime(eventMask e)
{
  rtc.adjust(DateTime(clock_now.year(), clock_now.month(), clock_now.day(), clockHours, clockMinutes, 0));
  return proceed;
}

PADMENU(timeMenu, "", doNothing, noEvent, noStyle, FIELD(clockHours, "", ":", 0, 23, 1, 0, saveTime, exitEvent, noStyle), FIELD(clockMinutes, "", "", 0, 59, 1, 0, saveTime, exitEvent, noStyle));
PADMENU(dateMenu, "", doNothing, noEvent, noStyle, FIELD(clockYear, "", "/", 1900, 2200, 10, 1, saveDate, exitEvent, noStyle), FIELD(clockMonth, "", "/", 1, 12, 1, 0, saveDate, exitEvent, noStyle), FIELD(clockDay, "", "", 1, 31, 1, 0, saveDate, exitEvent, noStyle));

MENU(clockSubmenu, "Clock", updateClockMenu, enterEvent, wrapStyle, SUBMENU(timeMenu), SUBMENU(dateMenu), EXIT("<Back"));

MENU(mainMenu, "Main menu", doNothing, noEvent, noStyle, EXIT("<Exit"), SUBMENU(setting_menu), SUBMENU(clockSubmenu), OP("Op A", doNothing, enterEvent), OP("Op B", doNothing, enterEvent), OP("Op C", doNothing, enterEvent));

#ifdef LCD216
MENU_OUTPUTS(out, MAX_DEPTH, LCD_OUT(lcd, {0, 0, 16, 2}), SERIAL_OUT(Serial)); //must have 2 items at least
#endif
#ifdef GLCD12864
MENU_OUTPUTS(out, MAX_DEPTH, U8G2_OUT(u8g2, colors, fontX, fontY, offsetX, offsetY, {0, 0, U8_Width / fontX, U8_Height / fontY}), SERIAL_OUT(Serial)); //must have 2 items at least
#endif

NAVROOT(nav, mainMenu, MAX_DEPTH, serial, out);

//buttons ------------------------------------------------------------------//
myButton buttons;
//voltage & Current --------------------------------------------------------//
ACS712 current_sensor(ACS712_30A, CURENT_PIN);
//Smoothed <float> battery_voltage_smooth;
//-------------------------------- setup  ----------------------------------//
void setup()
{
  buttons.init();
  //serial---------------
  Serial.begin(9600);
  Serial.println(F("Menu 4.x"));
  Serial.println(F("Use keys + - * /"));
  Serial.println(F("to control the menu navigation"));
  //DS1307---------------
  if (!rtc.begin())
    Serial.println(F("Couldn't find RTC"));
  if (!rtc.isrunning())
    Serial.println(F("RTC is NOT running!"));
#ifdef DS1307_SETUP
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
#endif
  // January 21, 2014 at 3am you would call:
  //rtc.adjust(DateTime(2014, 1, 21, 0, 5, 0));
  //---------------------
  //pinMode(_pin, OUTPUT);
  //voltage & Current ------------
  current_sensor.calibrate();
  Serial.println("current sensor is calibration");
  //battery_voltage_smooth.begin(SMOOTHED_AVERAGE, 10);
  //battery_voltage_smooth.clear();
  //lcd -------------------
#ifdef LCD216
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("start mess.....");
#endif
#ifdef GLCD12864
  u8g2.begin();
  u8g2.setFont(fontName);
#endif

  //ds18b20----------------
  ds18b20.begin();
  ds18b20.setResolution(DS18B20_RESOLUTION);
  ds18b20.setWaitForConversion(false);
  //if (!ds18b20.getAddress(DS18B20_ADDRESS1, 0))Serial.println("Unable to find address for Device 0");

  //menu-------------------
  nav.timeOut = 10;
  //nav.idleTask = idle; //point a function to be used when menu is suspended
  nav.idleOn();
#ifdef LCD216
  nav.showTitle = false;
#endif
#ifdef GLCD12864
  nav.showTitle = true;
#endif

  Serial.println(F("setup done."));
  Serial.flush();
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(10, 20, "GraphicsTest");
  u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
}

//------------------------------- loop  ------------------------------------//
void loop()
{
  time_cunter = millis() - time_last;
  time_last = millis();
  date_time_read();
  button_update();
  ds18b20_read();
  battery_read();
  data_to_array(log_temp, temperature_1_now);
  nav.doInput();
  /*
    if (nav.changed(0) || (nav.sleepTask && millis() - time_lcd_last > LCD_INTERVAL ))
    {
    u8g2.firstPage();
    do {
      draw();
    } while ( u8g2.nextPage() );
    }
  */
  if (nav.sleepTask && millis() - time_lcd_last > LCD_INTERVAL)
  {
    u8g2.firstPage();
    do
    {
      draw();
    } while (u8g2.nextPage());
    time_lcd_last = millis();
  }
  if (nav.changed(0))
  {
    u8g2.firstPage();
    do
    {
      nav.doOutput();
    } while (u8g2.nextPage());
    nav.doOutput();
  }
  /*
    if (nav.sleepTask && millis() - time_lcd_last > LCD_INTERVAL)
    {
    u8g2.clearBuffer();
    draw();
    nav.doOutput();
    u8g2.sendBuffer();
    time_lcd_last = millis();
    }
    if (nav.changed(0))
    {
    u8g2.clearBuffer();
    nav.doOutput();
    u8g2.sendBuffer();
    }*/
  //boiler_control();
  //Serial.println(time_cunter);
}

//-------------------------------- proges  ----------------------------------//

void date_time_read()
{
  clock_now = rtc.now();
}

void draw(void)
{
  switch (page_state)
  {
    case 0:
      page0();
      break;
    case 1:
      page1();
      break;
    case 2:
      page2();
      break;
    default:
      page2();
      break;
  }
}

void boiler_control()
{
  if (Boiler.isoff() && temperature_curent < boiler_temp_target - boiler_temp_Hysteresis)
  {
    Boiler.turnon();
  }
  if (Boiler.ison() && temperature_curent >= boiler_temp_target)
  {
    Boiler.turnoff();
  }
}

//button update
void button_update()
{
  buttons.update();
  if (buttons.onRelease(BUTTON_UP))
  {
    if (nav.sleepTask)
    {
      Serial.println(F("UP key"));
    }
    else
    {
      nav.doNav(upCmd);
    }
  }
  else if (buttons.onRelease(BUTTON_DOWN))
  {
    if (nav.sleepTask)
    {
      Serial.println(F("DOWN key"));
    }
    else
    {
      nav.doNav(downCmd);
    }
  }
  else if (buttons.onRelease(BUTTON_LEFT))
  {
    if (nav.sleepTask)
    {
      Serial.println(F("LEFT key"));
      page_previous();
    }
    else
    {
      nav.doNav(leftCmd);
    }
  }
  else if (buttons.onRelease(BUTTON_RIGHT))
  {
    if (nav.sleepTask)
    {
      Serial.println(F("RIGHT key"));
      page_next();
    }
    else
    {
      nav.doNav(rightCmd);
    }
  }
  else if (buttons.onRelease(BUTTON_SELECT))
  {
    if (nav.sleepTask)
    {
      Serial.println(F("SELECT key"));
    }
    else
    {
      nav.doNav(enterCmd);
    }
  }
}

//ds18b20 read
void ds18b20_read()
{
  if (millis() - time_ds18b20_last >= DS18B20_INTERVAL)
  {
    ds18b20.requestTemperaturesByAddress(ds18b20_address[0]); //Send the Command devices
    double tempC = ds18b20.getTempC(ds18b20_address[0]) + temperature_1_offset;
    temperature_1_now = tempC;
    if (temperature_1_now < temperature_1_min)
      temperature_1_min = temperature_1_now;
    if (temperature_1_now > temperature_1_max)
      temperature_1_max = temperature_1_now;
    temperature_curent = tempC;
    Serial.print(F("temperature_curent = "));
    Serial.println(temperature_curent);
    time_ds18b20_last = millis();
  }
}

void page_next()
{
  if (page_state < PAGE_NUMBER)
  {
    page_state++;
  }
  else
  {
    page_state = 0;
  }
}
void page_previous()
{
  if (page_state > 0)
  {
    page_state--;
  }
  else
  {
    page_state = PAGE_NUMBER - 1;
  }
}
#define status_bar_height 10
void page_frame()
{
  u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), status_bar_height);
}
void page_status_bar()
{
  u8g2.setFont(u8g2_font_battery19_tn);
  u8g2.setFontDirection(1);
  if (battery_level < 18)
    u8g2.drawGlyph(0, 0, 48);
  else if (battery_level >= 18 && battery_level < 36)
    u8g2.drawGlyph(0, 0, 49);
  else if (battery_level >= 36 && battery_level < 54)
    u8g2.drawGlyph(0, 0, 50);
  else if (battery_level >= 54 && battery_level < 72)
    u8g2.drawGlyph(0, 0, 51);
  else if (battery_level >= 72 && battery_level < 90)
    u8g2.drawGlyph(0, 0, 52);
  else
    u8g2.drawGlyph(0, 0, 53);
  u8g2.setFontDirection(0);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(21, 10);
  u8g2.print(battery_level);
  u8g2.print("% ");
  u8g2.setCursor(91, 10);
  u8g2.print(' ');
  u8g2.print(clock_now.hour(), DEC);
  u8g2.print(':');
  u8g2.print(clock_now.minute(), DEC);
}

void data_to_array(int8_t arry[], double data)
{
  uint8_t i = ((clock_now.hour() * 60) + clock_now.minute()) / SWHO_LOG_PERIOD;
  if (data >= 128)
  {
    arry[i] = 128;
  }
  else if (data <= -128)
  {
    arry[i] = -128;
  }
  else
  {
    arry[i] = data; //range of -32,768 to 32,767
  }
}
//test
void array_print(int8_t arry[])
{
  Serial.print("array={");
  for (uint8_t i = 0; i < 10; i++)
  {
    if (i % 8 == 0)
    {
      Serial.println(" ");
    }
    Serial.print(arry[i]);
    Serial.print(" ;");
  }
  Serial.println("}");
}
void drawGraph()
{
  uint8_t pos_x = 20;
  uint8_t pos_Y = 11;
  uint8_t width = 96;
  uint8_t height = 42;
  int8_t val_max = log_temperature.getMax();
  int8_t val_min = log_temperature.getMin();
  int8_t val_per = 0;
  Serial << "max= " << val_max << "\t min= " << val_min;
  array_print(log_temp);
  //u8g2.drawFrame(pos_x - 1, pos_Y - 1, width + 1, height + 1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(0, 9);
  u8g2.print("histogrsm temperature");
  u8g2.setCursor(0, pos_Y + 10);
  u8g2.print(val_max);
  u8g2.setCursor(0, pos_Y + height);
  u8g2.print(val_min);

  //u8g2.setFont(u8g2_font_battery19_tn);
  //u8g2.setFontDirection(1);
  //u8g2.drawGlyph(0, 0, 48);

  u8g2.setCursor(0, 64);
  u8g2.print("clock");
  u8g2.setCursor(pos_x + 24, 64);
  u8g2.print("6");
  u8g2.setCursor(pos_x + 48, 64);
  u8g2.print("12");
  u8g2.setCursor(pos_x + 72, 64);
  u8g2.print("18");
  u8g2.setCursor(pos_x + 96, 64);
  u8g2.print("24");

  for (uint8_t i = 0; i < width; i++)
  {
    int8_t val = log_temperature[i];
    int8_t val_map = map(val, val_min, val_max, 0, height);
    if (i > 0) {
      u8g2.drawLine(i + pos_x - 1 , pos_Y + height - val_per, i + pos_x , pos_Y + height - val_map);
    }
    //u8g2.drawPixel(i + pos_x , pos_Y + height - val_map);
    //u8g2.drawVLine(i + pos_x, pos_Y + height - val_map, val_map);
    val_per = val_map;
  }
}
void page0()
{
#ifdef LCD216
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("timeL= "));
  lcd.print(time_cunter);
  lcd.setCursor(0, 1);
  //lcd.print(F("                "));
  lcd.print(F("temp= "));
  lcd.print(temperature_1_now);
#endif
#ifdef GLCD12864
  page_status_bar();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(0, 40);
  u8g2.print(F("voltage = "));
  u8g2.print(battery_voltage);
  u8g2.print(" V");
  u8g2.setCursor(0, 52);
  u8g2.print(F("power="));
  u8g2.print(battery_power);
  u8g2.print("watt ");
  u8g2.setCursor(0, 64);
  u8g2.print(F("current ="));
  u8g2.print(battery_current);
  u8g2.print(" A");
#endif
}
void page1()
{
#ifdef LCD216
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("PWM= "));
  lcd.print(Boiler.getout());
  lcd.print(F("S="));
  lcd.print(Boiler.getState());
  lcd.setCursor(0, 1);
  //lcd.print(F("                "));
  lcd.print(F("p1= "));
  lcd.print(temperature_1_now);
#endif
#ifdef GLCD12864
  //u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight() );
  drawGraph();
#endif
}
void page2() {
#ifdef LCD216
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Temp min "));
  lcd.print(temperature_1_min);
  lcd.setCursor(0, 1);
  lcd.print(F("Temp max "));
  lcd.print(temperature_1_max);
#endif
#ifdef GLCD12864
  u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
  u8g2.drawStr(0, 20, "drawRFrame/Box");
  u8g2.setCursor(10, 62);
  u8g2.print("temp = ");
  u8g2.print(temperature_1_now);
#endif
}

void battery_read()
{
  if (millis() - time_battery_last >= BATTERY_INTERVAL)
  {
    float vout = (analogRead(VOLTAG_PIN) * 5.0) / 1024.0;
    battery_voltage = vout / (VOLTAG_R2 / (VOLTAG_R1 + VOLTAG_R2));
    //battery_voltage = battery_voltage_smooth.add(vin);
    battery_level = (battery_voltage - VOLTAG_BATT_MIN) * 100 / (VOLTAG_BATT_MAX - VOLTAG_BATT_MIN);
    battery_current = current_sensor.getCurrentDC();
    if (battery_current < 0)
    {
      current_sensor.calibrate();
      battery_current = 0;
    }
    battery_power += battery_current * battery_voltage;
    time_battery_last = millis();
  }
}
