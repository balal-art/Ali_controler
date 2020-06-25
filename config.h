/*
  Written by: Ali Hosseini <balal.art@gmail.com>
  1398/10/14
  Version 1.0
*/
//test
#define MYDEBUG
//#define HAMED
///////////////////////////input divice///////////////////////////
#define MEGA2560
///////////////////////////input divice///////////////////////////
#define BUTTON_RIGHT  3
#define BUTTON_UP     2
#define BUTTON_DOWN   1
#define BUTTON_LEFT   0
#define BUTTON_SELECT 4
#define BUTTON_NON    5
#define BUTTONS_VALUES {0, 129, 317, 484, 711, 1023}//Ali
////////////////////////////pinout///////////////////////////////

// PIN ON MEGA 2560
// PIN definitions     / assignments  / Description
//Open Pin                      0     // USB RX Serial Pin
//Open Pin                      1     // USB TX Serial Pin
#define BOILER_PIN              2     // PWM
//Open Pin                      3     // PWM
#define D4_PIN                  4     // PWM
#define D5_PIN                  5     // PWM
#define D6_PIN                  6     // PWM
#define D7_PIN                  7     // PWM
#define D8_PIN		              8     // PWM
#define D9_PIN                  9     // PWM
#define D10_PIN                 10    // PWM
//Open Pin                      11    // PWM
//Open Pin                      12    // PWM
//LED Pin                       13    // PWM LED BOARD

//Open Pin                      14    // TX3
//Open Pin                      15    // RX3
//Open Pin                      16    // TX2
//Open Pin                      16    // RX2
#define GSM_TX                  18    // TX1
#define GSM_RX                  19    // RX1 
//Open Pin                      20    // SDA
//Open Pin                      21    // SCL
//Open Pin                      22    //
#define DS18B20_PIN             23    // dS18B20
//Open Pin                      24    //
//Open Pin                      25    //
//Open Pin                      26    //
//Open Pin                      27    //
//Open Pin                      28    //
//Open Pin                      29    //
//Open Pin                      30    //
//Open Pin                      31    //
//Open Pin                      32    //
//Open Pin                      33    //
//Open Pin                      34    //
//Open Pin                      35    //
//Open Pin                      36    //
//Open Pin                      37    //
//Open Pin                      38    //
//Open Pin                      39    //
//Open Pin                      40    //
//Open Pin                      41    //
//Open Pin                      42    //
//Open Pin                      43    //
//Open Pin                      44    //PWM
//Open Pin                      45    //PWM
//Open Pin                      46    //PWM
//Open Pin                      47    //
//Open Pin                      48    //
//Open Pin                      49    //
#define MYMISO_PIN                50    //SPI MISO
#define MYMOSI_PIN                51    //SPI MOSI 
#define MYSCK_PIN                 52    //SPI SCK 
#define MYSS_PIN                  53    //SPI SS

#define BUTTONS_PIN             A0    // keypad 5 buttons
#define LDR_IN_PIN              A1    // in Photo resistor
#define LDR_OUT_PIN             A2    // out Photo resistor
//Open Pin                      A3    //
//Open Pin                      A4    //I2C
//Open Pin                      A5    //I2C
#define VOLTAG_PIN              A6    // 
#define CURENT_PIN              A7    // 
//Open Pin                      A8    //
//Open Pin                      A9    //
//Open Pin                      A10   //
//Open Pin                      A12   //
//Open Pin                      A13   //
//Open Pin                      A14   //
//Open Pin                      A15   //

///////////////////////////lcd device/////////////////////////////
//#define LCD216
#define GLCD12864
#define LCD_INTERVAL  200
#define SHOW_TITLE  true
#ifdef  LCD216
#define LCD_D4                  D4_PIN     // PWM
#define LCD_D5                  D5_PIN     // PWM
#define LCD_D6                  D6_PIN     // PWM
#define LCD_D7                  D7_PIN     // PWM
#define LCD_RS                  D8_PIN     // PWM
#define LCD_ENABLE              D9_PIN    // PWM
#define LCD_BACKLIGHT           D10_PIN
#endif
#ifdef GLCD12864
#define GlCD_E    MYSCK_PIN   //52 orang
#define GLCD_RW   MYMOSI_PIN  //51 blue
#define GLCD_RS   D10_PIN   //10 yellow
#define GLCD_RST  D8_PIN    //8 green
#endif
////////////////////ds18b20 onewire sensor/////////////////////////
#define DS18B20_INTERVAL 2000 // defult is 1000 millis
//9 → 0.5° 93.75ms || 10 → 0.25° 187.5ms || 11 → 0.125° 375ms || 12 → 0.0625° 750ms
#define DS18B20_RESOLUTION 12 //set the resolution to 10 bit ADC defult sensor is 12

// assign address manually.  the addresses below will beed to be changed
// to valid device addresses on your bus.  device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
//#define DS18B20_ADDRESS1 {28,AA,F5,96,3B,14,01,87}


///////////////////////voltage & curent ////////////////////////
//#define VOLTAG_PIN  A6
#define BATTERY_CAPICITY 720 //12v*60A=720watt
#define VOLTAG_R1 47000.0
#define VOLTAG_R2 10000.0
#define VOLTAG_REPET 20
#define VOLTAG_BATT_MAX 13.1
#define VOLTAG_BATT_MIN 10.5
#define BATTERY_INTERVAL 2000
//////////////////////////// eeprom /////////////////////////////

#define DS1307_ADDRESS 0x50

//////////////////////////// ds1307 /////////////////////////////
//#define DS1307_SETUP
//#define DS1307_ADDRESS 0x68

//////////////////////////// logger /////////////////////////////
#define SWHO_LOG_PERIOD    15   //1 minutes for test defult 15 minutes
#define SWHO_LOG_PER_HOUR  4    //(60/SWHO_LOG_PERIOD)
#define SWHO_LOG_COUNT     96   //(24*SWHO_LOG_PER_HOUR)
#define SWHO_LOG_ARRAY    21,21,21,21,22,22,23,24,23,23,22,22,23,23,21,21
