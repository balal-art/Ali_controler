
#ifndef MY_BUTTON_H
#define MY_BUTTON_H
#include <Arduino.h>
#include "config.h"
#define MYBUTTONS_PIN  BUTTONS_PIN
#define MYBUTTONS_RESOLUTION 1024
#define MYBUTTONS_DEBOUNCE_DURATION 20
#define MYBUTTONS_MAX_BUTTONS 6
#define MYBUTTONS_TOTAL 6
#define MYBUTTONS_VALUES BUTTONS_VALUES //{0, 129, 317, 484, 711, 1023}//Ali

class myButton
{
  public:
    myButton();
    void init();
    boolean isPressed(int button) {
      return buttonPressed == button;  // evaluates to true continually while <button> is pressed
    }
    boolean isPressedBefore(int button, int duration); // called continually while <button> is pressed for less than <duration> (ms)
    boolean isPressedAfter(int button, int duration); // called continually while <button> is pressed for longer than <duration> (ms)
    boolean onPress(int button) {
      return buttonOnPress == button;  // evaluates to true for one update cycle after <button> is pressed
    }
    boolean onPressAfter(int button, int duration); // evaluates to true for one update cycle after <button> is pressed for longer than <duration> (ms)
    boolean onPressAndAfter(int button, int duration); // evaluates to true for one update cycle after <button> is pressed, and again once it has been pressed for longer than <duration> (ms)
    boolean onPressAfter(int button, int duration, int repeatTime); // evaluates to true for one update cycle after <button> is pressed for longer than <duration>, and then repeatedly after that every <repeatTime> milliseconds
    boolean onPressAndAfter(int button, int duration, int repeatTime); // evaluates to true for one update cycle after <button> is pressed, again when pressed for longer than <duration>, and then repeatedly after that every <repeatTime> milliseconds
    boolean onRelease(int button)  {
      return buttonOnRelease == button;  // evaluates to true for one update cycle after <button> is released
    }
    boolean onReleaseBefore(int button, int duration);  // evaluates to true for one update cycle after <button> is released, only if it was pressed for shorter than <duration>
    boolean onReleaseAfter(int button, int duration);  // evaluates to true for one update cycle after <button> is released, only if it was pressed for longer than or equal to <duration>
    int getPressDuration(); // gets the duration that the current button has been pressed for, in milliseconds
    int getLastReleasePressDuration() {
      return millis() - releasedButtonPressTime;  // gets the duration that the last released button was pressed for, in milliseconds
    }

    void update();

  private:
    //int pin;
    //int total;
    int valueBoundaries[MYBUTTONS_MAX_BUTTONS];

    int buttonPressed = -1;
    int buttonOnPress = -1;
    int buttonOnRelease = -1;

    unsigned long thisUpdateTime = 0;
    unsigned long lastUpdateTime = 0;
    unsigned long buttonPressTime = 0;
    unsigned long releasedButtonPressTime = 0;
    int lastDebounceButton = -1;
    unsigned long lastDebounceButtonTime = 0;

    int getButtonForAnalogValue(int value);
    boolean debounceButton(int button);
};

#endif
