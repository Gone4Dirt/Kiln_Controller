/*
    class for displaying data/info on the OLED display 
*/

#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Screen
{
    public:
        bool init(void);

        void update(int16_t target, int16_t measured);

        void error(void);

        void measurement_error(uint8_t fault_code);

    private:

        Adafruit_SSD1306* _display;

        unsigned long _error_set_ms;

        // Do not allow copies of Screen by keeping the operators private
        Screen& operator=(const Screen&);

};
#endif
