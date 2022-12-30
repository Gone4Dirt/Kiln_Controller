/*
    class for reading in thermal profile to be used 
*/

#ifndef PROFILE_H
#define PROFILE_H

#define MAX_PROFILE_POINTS 20

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class ThermalProfile
{
    public:

        bool init(void);

        int16_t get_desired_temp(void);

    private:

        // Thermal profile definition
        int16_t _durations [MAX_PROFILE_POINTS];
        int16_t _temps [MAX_PROFILE_POINTS];
        uint8_t _profile_position;

        // The temperature target defined in the last profile step
        int16_t _last_temp_target;

        unsigned long _last_profile_step_ms;

        int16_t to_int(char *c, uint8_t n);

};
#endif
