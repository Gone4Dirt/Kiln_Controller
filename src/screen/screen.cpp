#include "screen.h"

#define DEBUG true
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define ADDRESS 0x3C // I2C Address of screen
#define ERROR_DISPLAY_TIME 5000 // time that the error screen must stay on screen





/// @brief Intialise display
/// @param None
/// @return bool if loading profile is succesful or not
bool Screen::init(void)
{

    _display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

    if(!_display->begin(SSD1306_SWITCHCAPVCC, ADDRESS) && DEBUG) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }

    delay(2000);

    _display->clearDisplay();

    _display->setTextSize(2);
    _display->setTextColor(WHITE);

    _display->setCursor(0, 5);
    _display->print("T:");

    _display->setCursor(0, 32);
    _display->print("M:");

    _display->display(); 

    return true;
}

void Screen::update(int16_t target, int16_t measured)
{

    if (millis() - _error_set_ms <= ERROR_DISPLAY_TIME) {
        // we are not allowd to overide the display yet
        return;
    }

    _display->clearDisplay();

    // _display->setTextSize(2);
    // _display->setTextColor(WHITE);

        // Print the target temp
    _display->setCursor(0, 5);
    _display->print("T: ");

    // print the number
    _display->print(target);

    // print the units
    _display->print(" ");
    _display->setTextSize(1);
    _display->cp437(true); // Need code page font 437
    _display->write(167);
    _display->setTextSize(2);
    _display->print("C");


    // Print the measured temp
    _display->setCursor(0, 32);
    _display->print("M: ");

    // print the number
    _display->print(measured);

    // print the little circle for degree
    _display->print(" ");
    _display->setTextSize(1);
    // _display->cp437(true); // Need code page font 437
    _display->write(167);
    _display->setTextSize(2);
    _display->print("C");

    _display->display(); 

}

void Screen::error(void)
{
    _display->clearDisplay();
    _display->setTextSize(2);
    _display->setCursor(15, 22);
    _display->print("ERROR!!!");
    _display->display(); 

    // Prevent the display being overridden by setting the timer
    _error_set_ms = millis();
}


