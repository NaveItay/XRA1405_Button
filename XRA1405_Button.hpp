/**
 * @file
 *    XRA1405_Button.hpp
 *
 * @brief
 *    This library provides an interface for debouncing and detecting button states,
 *    counting button presses, and detecting double-click events using the XRA1405 SPI GPIO expander.
 *
 * Key Features:
 *    - Debouncing of button inputs to mitigate the effects of contact bounce.
 *    - Counting the number of button presses with configurable counting modes (rising edge, falling edge, or both).
 *    - Detecting double-click events with a configurable interval.
 *    - Reading the stable state of the button after debouncing.
 *    - Getting the immediate state of the button without debouncing for real-time applications.
 *
 * Usage:
 *    - Create an XRA1405_MyButton instance specifying the chip select pin and button pin connected to the XRA1405.
 *    - Call the loop method in the main loop to update the button state.
 *    - Use getCount to retrieve the number of button presses, and clearCount to reset the count.
 *    - Use checkPressed and checkReleased to check for button press and release events.
 *    - Use checkDoubleClick to detect double-click events.
 *
 * Example:
 *    ```
 *    XRA1405_MyButton myButton(ESP32_GPIO15_HSPI_CS1, XRA1405_BUTTON_PIN);
 *    myButton.configureDebounceDuration(50);  // Set debounce time to 50ms
 *    myButton.configureCountingMode(XRA1405_COUNT_FALLING);  // Count on falling edge
 *
 *    void loop() {
 *      myButton.loop();  // Update button state
 *
 *      if (myButton.checkPressed()) {
 *        Serial.println("Button pressed!");
 *      }
 *
 *      if (myButton.checkDoubleClick()) {
 *        Serial.println("Double click detected!");
 *      }
 *    }
 *    ```
 *
 * @author
 *    Itay Nave, Embedded Software Engineer
 * @date
 *    3/24/2024
 *
 * @copyright
 *    © 2023 Itay Nave. All rights reserved.
 *    This software is provided "as is", without warranty of any kind, express or implied.
 *    Redistribution and use in source and binary forms, with or without modification,
 *    are permitted provided that the above copyright notice and this permission notice
 *    appear in all copies.
 */

#ifndef XRA1405_BUTTON_HPP
#define XRA1405_BUTTON_HPP

#include <Arduino.h>

enum XRA1405_CountMode
{
    XRA1405_COUNT_FALLING = 0,
    XRA1405_COUNT_RISING = 1,
    XRA1405_COUNT_BOTH = 2
};

class XRA1405_MyButton
{
public:
    XRA1405_MyButton(uint8_t chipSelectPin, uint8_t button_pin);
    XRA1405_MyButton(uint8_t chipSelectPin, uint8_t button_pin, uint8_t gpio_mode);

    void XRA1405_loop(void);
    void XRA1405_setLoopIntervalMicros(unsigned long interval); // New function to set the loop interval in microseconds

    void XRA1405_configureDebounceDuration(unsigned long time);
    int XRA1405_getStableState(void);
    uint8_t XRA1405_getImmediateState(void);
    bool XRA1405_checkPressed() const;  // Mark as const
    bool XRA1405_checkReleased() const; // Mark as const
    void XRA1405_configureCountingMode(XRA1405_CountMode mode);
    unsigned long XRA1405_getCount(void);
    void XRA1405_clearCount(void);

    // DoubleClick and loop function cannot use both!
    bool XRA1405_checkDoubleClick();

private:
    uint8_t _chipSelectPin;
    uint8_t _button_pin;
    uint8_t _gpio_mode;

    unsigned long _debounce_time;
    unsigned long _count;
    int _count_mode;

    int _previous_steady_state;
    int _last_steady_state;
    int _last_flickerable_state;

    unsigned long _last_debounce_time;

    // checkDoubleClick
    unsigned long _last_click_time;
    bool _first_click_detected;

    // New members for microsecond interval timing
    unsigned long _loopIntervalMicros = 1000; // Default interval of 1000 microseconds (1 millisecond)
    unsigned long _lastUpdateTime = 0;        // Tracks the last update time in microseconds
};

#endif // XRA1405_BUTTON_HPP