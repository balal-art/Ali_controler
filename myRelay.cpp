#include "myRelay.h"
#include "myTypes.h"


myRelay::myRelay(int pinout)
{
	_pin = pinout;
}

void myRelay::begin()
{
	pinMode(_pin, OUTPUT);
}

bool myRelay::isoff()
{
  if(_state==OUT_OFF)
  {
  return true;
  }
  else
  {
    return false;
  }
}

bool myRelay::ison()
{
  if(_state==OUT_ON)
  {
  return true;
  }
  else
  {
    return false;
  }
}


enum Out_State myRelay::getState()
{
	return _state;
}

uint8_t myRelay::getout()
{
	return _pwm_out;
}

void myRelay::turnon()
{
	if (_state == OUT_TURNOFF || _state == OUT_OFF)
	{
		_state = OUT_TURNON;
		_last_time_on = millis();
	}
	if (_state == OUT_TURNON)
	{
		if (relay_type == MOSFEET_DRIVE || relay_type == MOSFEET)
		{
			_on_mosfeet();
		}
		else
		//if(relay_type == RELAYNC || relay_type == RELAYNC_DRIVE || relay_type == RELAYNO || relay_type == RELAYNO_DRIVE)
		{
			_on_mosfeet();
		}
		analogWrite(_pin, _pwm_out);
	}
}

void myRelay::turnoff()
{
	if (_state == OUT_TURNON || _state == OUT_ON)
	{
		_state = OUT_TURNOFF;
		_last_time_off = millis();
	}
	if (_state == OUT_TURNOFF)
	{
		if (relay_type == MOSFEET_DRIVE || relay_type == MOSFEET)
		{
			_off_mosfeet();
		}
		else
		//if(relay_type == RELAYNC || relay_type == RELAYNC_DRIVE || relay_type == RELAYNO || relay_type == RELAYNO_DRIVE)
		{
			_off_mosfeet();
		}
		analogWrite(_pin, _pwm_out);
	}
}

void myRelay::_off_mosfeet() //turn off
{
	int _remind_time = off_delay - (millis() - _last_time_off);
	if (_remind_time > 0)
	{
		_pwm_out = map(_remind_time, 0, off_delay, pwm_off_min, pwm_off_max);
	}
	else
	{
		_pwm_out = 0;
		_state = OUT_OFF;
	}
}
void myRelay::_on_mosfeet() //turn off
{
	int _remind_time = on_delay - (millis() - _last_time_on);
	if (_remind_time > 0)
	{
		_pwm_out = map(_remind_time, 0, on_delay, pwm_on_min, 255);
	}
	else
	{
		_pwm_out = pwm_on_min;
		_state = OUT_ON;
	}
}
