#ifndef TEMP_MEASURE_H
#define TEMP_MEASURE_H

#include <Arduino.h>
#include <Adafruit_MAX31856.h>

#define THERM_CS 46        // Chip select pin for MAX 31856

class TempMeasure
{
    public:
        TempMeasure(void);

        void init(void);

        bool update(void);

    private:

        Adafruit_MAX31856 _thermocouple = Adafruit_MAX31856(THERM_CS);

        bool has_fault(void);

        float therm_temp;
        float board_temp;

        unsigned long last_update_ms;

};
#endif
