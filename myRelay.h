// Relay.h

#ifndef _MYRELAY_h
#define _MYRELAY_h

#include "Arduino.h"
#include "myTypes.h"

class myRelay
{
  public:
    //myRelay();
    //Relay(int p, bool isNormallyOpen);
    myRelay(int pinout);
    enum Controled_Type relay_type = MOSFEET_DRIVE;
    uint8_t pwm_on_min 	= 120;
    uint16_t on_delay 	= 1000; // unit millis second
    uint8_t pwm_off_max = 120;
    uint8_t pwm_off_min = 10;
    uint16_t off_delay 	= 8000; // unit millis second
    uint8_t off_repet 	= 0;
    void begin();
    bool isoff();
    bool ison();
    enum Out_State getState();
    uint8_t getout();
    void turnon();
    void turnoff();

  protected:
    int _pin;
    enum Out_State _state = OUT_OFF;
    unsigned long _last_time_on = 0;
    unsigned long _last_time_off = 0;
    int _pwm_out = 0;
    void _off_mosfeet();
    void _on_mosfeet();
};

#endif
