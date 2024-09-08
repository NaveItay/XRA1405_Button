#include <SPI.h>
#include <XRA1405_Button.hpp>

// Define SPI and button pins
#define SCK 18
#define MISO 19
#define MOSI 23
#define SS 5
#define BUTTON_PIN 3

XRA1405_MyButton myButton(SS, BUTTON_PIN);

void setup()
{
    Serial.begin(115200);
    XRA1405_begin(SCK, MISO, MOSI);
    myButton.XRA1405_configureDebounceDuration(50);                // Set debounce time to 50ms
    myButton.XRA1405_configureCountingMode(XRA1405_COUNT_FALLING); // Count on falling edge (button press)
}

void loop()
{
    myButton.XRA1405_loop();

    if (myButton.XRA1405_checkPressed())
    {
        Serial.println("Button pressed!");
    }

    if (myButton.XRA1405_checkReleased())
    {
        Serial.println("Button released!");
    }

    // Print the button press count
    unsigned long pressCount = myButton.XRA1405_getCount();
    Serial.print("Button press count: ");
    Serial.println(pressCount);
}
