#ifndef TEMP_MEASURE_H
#define TEMP_MEASURE_H

#include <Arduino.h>
#include <Adafruit_MAX31856.h>
#include "../logger/logger.h"

#define THERM_CS 53        // Chip select pin for MAX 31856

class TempMeasure
{
    public:
        TempMeasure(void);

        void init(Logger *ptr);

        bool update(void);

        float const get_measured_temp() { return therm_temp; }

        uint8_t const get_fault_code(void) { return _fault_code; }

    private:

        Adafruit_MAX31856 _thermocouple = Adafruit_MAX31856(THERM_CS);

        bool check_faults(uint8_t &fault);

        float therm_temp;
        float board_temp;
        uint8_t _fault_code;

        unsigned long last_update_ms;

        Logger *_logger;

};
#endif
