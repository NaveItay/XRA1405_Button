#include <XRA1405_Button.hpp>

#include "XRA1405.hpp"

const unsigned long DOUBLE_CLICK_INTERVAL = 500; // Time in milliseconds

XRA1405_MyButton::XRA1405_MyButton(uint8_t chipSelectPin, uint8_t button_pin) : XRA1405_MyButton(chipSelectPin, button_pin, INPUT) {};

XRA1405_MyButton::XRA1405_MyButton(uint8_t chipSelectPin, uint8_t button_pin, uint8_t gpio_mode)
{
    _chipSelectPin = chipSelectPin;
    _button_pin = button_pin;
    _gpio_mode = gpio_mode;

    _debounce_time = 0;
    _count = 0;
    _count_mode = XRA1405_COUNT_FALLING;

    XRA1405_pinMode(chipSelectPin, button_pin, gpio_mode);

    _previous_steady_state = XRA1405_digitalRead(_chipSelectPin, _button_pin);
    _last_steady_state = _previous_steady_state;
    _last_flickerable_state = _previous_steady_state;

    _last_debounce_time = 0;

    // DoubleClick
    unsigned long _last_click_time;
    bool _first_click_detected;
}

void XRA1405_MyButton::XRA1405_loop(void)
{
    /// @todo delete this block
    // unsigned long currentTimeMicros = micros();
    // // Check if the loopIntervalMicros has passed since the last update
    // if (currentTimeMicros - _lastUpdateTime < _loopIntervalMicros)
    // {
    //     return; // Not enough time has passed, exit the function
    // }
    // _lastUpdateTime = currentTimeMicros; // Update the last update time

    // Current time for debounce calculations
    unsigned long currentTimeMillis = millis();

    // Read the current state of the button.
    int currentState = XRA1405_digitalRead(_chipSelectPin, _button_pin);

    // If the current state is different from the last flickerable state,
    // it could be due to a button press or noise.
    if (currentState != _last_flickerable_state)
    {
        // Reset the debounce timer to the current time.
        _last_debounce_time = currentTimeMillis;
        // Update the last flickerable state to the current state.
        _last_flickerable_state = currentState;
    }

    // Check if the button state has been stable for longer than the debounce time.
    if ((currentTimeMillis - _last_debounce_time) >= _debounce_time)
    {
        // Update the steady state of the button if the debounce period has passed.
        // This helps in filtering out any noise or short fluctuations.
        _previous_steady_state = _last_steady_state;
        _last_steady_state = currentState;
    }

    // Check for state change after the debounce period.
    if (_previous_steady_state != _last_steady_state)
    {
        // Increment the button press count based on the counting mode.
        // COUNT_BOTH: Increment on both rising and falling edges.
        // COUNT_FALLING: Increment on the falling edge (HIGH to LOW transition).
        // COUNT_RISING: Increment on the rising edge (LOW to HIGH transition).
        if ((_count_mode == XRA1405_COUNT_BOTH) ||
            (_count_mode == XRA1405_COUNT_FALLING && _previous_steady_state == HIGH && _last_steady_state == LOW) ||
            (_count_mode == XRA1405_COUNT_RISING && _previous_steady_state == LOW && _last_steady_state == HIGH))
        {
            _count++;
        }
    }
}

void XRA1405_MyButton::XRA1405_setLoopIntervalMicros(unsigned long interval)
{
    _loopIntervalMicros = interval;
}

void XRA1405_MyButton::XRA1405_configureDebounceDuration(unsigned long time)
{
    _debounce_time = time;
}

int XRA1405_MyButton::XRA1405_getStableState(void)
{
    return _last_steady_state;
}

uint8_t XRA1405_MyButton::XRA1405_getImmediateState(void)
{
    return XRA1405_digitalRead(_chipSelectPin, _button_pin);
}

bool XRA1405_MyButton::XRA1405_checkPressed(void) const
{
    return (_previous_steady_state == HIGH && _last_steady_state == LOW);
}

bool XRA1405_MyButton::XRA1405_checkReleased(void) const
{
    return (_previous_steady_state == LOW && _last_steady_state == HIGH);
}

void XRA1405_MyButton::XRA1405_configureCountingMode(XRA1405_CountMode mode)
{
    _count_mode = mode;
}

unsigned long XRA1405_MyButton::XRA1405_getCount(void)
{
    return _count;
}

void XRA1405_MyButton::XRA1405_clearCount(void)
{
    _count = 0;
}

bool XRA1405_MyButton::XRA1405_checkDoubleClick()
{
    int currentState = XRA1405_digitalRead(_chipSelectPin, _button_pin);
    unsigned long currentTime = millis();
    bool doubleClicked = false;

    // Check for any state change
    if (currentState != _last_flickerable_state)
    {
        _last_flickerable_state = currentState; // Update the last flickerable state

        // Detect state change from HIGH to LOW (button press)
        if (currentState == LOW)
        { // Assuming active-low button
            if (!_first_click_detected)
            {
                _first_click_detected = true;
                _last_click_time = currentTime;
            }
            else if (currentTime - _last_click_time <= DOUBLE_CLICK_INTERVAL)
            {
                // Double click detected
                doubleClicked = true;
                _first_click_detected = false; // Reset for next detection
            }
        }
    }

    // Reset first_click_detected if time exceeded and no second click
    if (_first_click_detected && (currentTime - _last_click_time > DOUBLE_CLICK_INTERVAL))
    {
        _first_click_detected = false;
    }

    return doubleClicked;
}