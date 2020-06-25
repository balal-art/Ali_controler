
#include <Arduino.h>
#include "myButton.h"

myButton::myButton()
{
}

void myButton::init()
{
  pinMode(MYBUTTONS_PIN, INPUT ); // ensure button pin is an input
  digitalWrite(MYBUTTONS_PIN, LOW ); // ensure pullup is off on button pin

  const int values[] = MYBUTTONS_VALUES ;
  for (int i = 0; i < MYBUTTONS_TOTAL; i++) {
    // determine value boundaries, so we can easily determine which button has the closest value to the current analogRead()
    // for example if values were {100, 200, 300}, then we want any value from 0-149 to trigger button 1, 150-245 to trigger button 2 etc...
    int nextValue;
    if (i + 1 < MYBUTTONS_TOTAL)
    {
      nextValue = values[i + 1];
    }
    else
    {
      nextValue = MYBUTTONS_RESOLUTION;
    }
    valueBoundaries[i] = (values[i] + nextValue) * 0.5;
  }
}

void myButton::update()
{
  buttonOnPress = -1;
  buttonOnRelease = -1;
  lastUpdateTime = thisUpdateTime;
  thisUpdateTime = millis();

  int a = analogRead(MYBUTTONS_PIN);
  int button = getButtonForAnalogValue(a);
  if (debounceButton(button) && button != buttonPressed)
  {
    releasedButtonPressTime = buttonPressTime;

    if (button != -1)
      buttonPressTime = thisUpdateTime;

    buttonOnPress = button;
    buttonOnRelease = buttonPressed;

    buttonPressed = button;
  }
}

boolean myButton::isPressedBefore(int button, int duration)
{
  return buttonPressed == button && (thisUpdateTime < duration + buttonPressTime);
}

boolean myButton::isPressedAfter(int button, int duration)
{
  return buttonPressed == button && (thisUpdateTime >= duration + buttonPressTime);
}

boolean myButton::onPressAfter(int button, int duration)
{
  unsigned long delayedPressTime = duration + buttonPressTime;
  return buttonPressed == button && (thisUpdateTime >= delayedPressTime) && (lastUpdateTime < delayedPressTime);
}

boolean myButton::onPressAndAfter(int button, int duration)
{
  return onPress(button) || onPressAfter(button, duration);
}

boolean myButton::onPressAfter(int button, int duration, int repeatTime)
{
  int a = (int(thisUpdateTime - buttonPressTime) - duration + int(repeatTime * 0.5)) / repeatTime;
  if (a < 0)
    a = 0;

  unsigned long delayedPressTime = duration + buttonPressTime + repeatTime * a;
  return buttonPressed == button && (thisUpdateTime >= delayedPressTime) && (lastUpdateTime < delayedPressTime);
}

boolean myButton::onPressAndAfter(int button, int duration, int repeatTime)
{
  return onPress(button) || onPressAfter(button, duration, repeatTime);
}

boolean myButton::onReleaseBefore(int button, int duration)
{
  return buttonOnRelease == button && (thisUpdateTime < duration + releasedButtonPressTime);
}

boolean myButton::onReleaseAfter(int button, int duration)
{
  return buttonOnRelease == button && (thisUpdateTime >= duration + releasedButtonPressTime);
}

int myButton::getPressDuration()
{
  if (buttonPressed == -1)
    return 0;

  return thisUpdateTime - buttonPressTime;
}

int myButton::getButtonForAnalogValue(int value)
{
  for (int i = 0; i < MYBUTTONS_TOTAL; i++) {
    if (value < valueBoundaries[i])
      return i;
  }
  return -1;
}

boolean myButton::debounceButton(int button)
{
  if (button != lastDebounceButton)
    lastDebounceButtonTime = thisUpdateTime;

  lastDebounceButton = button;
  return (thisUpdateTime - lastDebounceButtonTime > MYBUTTONS_DEBOUNCE_DURATION);
}
