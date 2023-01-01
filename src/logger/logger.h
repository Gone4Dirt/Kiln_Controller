#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class Logger
{
    public:

        void init(void);

        void write(void);

        void log_therm_profile(unsigned long time_ms, int16_t dest);

        void log_measurement(unsigned long time_ms, float bt, float tt, uint8_t flt);

        void log_controller(unsigned long time_ms, float p, float i, float d, bool limit_flag);

        void log_output(float out, uint16_t win);

    private:
        // Track the status of the sd card state
        enum Status {
          NO_FAULT,
          SD_INIT_FAILED,
          SD_FILE_START_ERROR,
          SD_WRITE_FAULT
        };

        File _sd_file;                          // SD card write object

        unsigned long _last_update_ms;

        // Thermal profile
        unsigned long _therm_prof_time_ms;
        int16_t _desired_temp;

        // Measurement
        unsigned long _measure_time_ms;
        float _board_temp;
        float _thermocouple_temp;
        uint8_t _fault;

        // Controller
        unsigned long _ctrl_time_ms;
        float _p;
        float _i;
        float _d;
        bool _limit_flag;

        // Output to SSR
        float _output;
        uint16_t _win;

        // Functions
        void raise_logger_error(Status err);

        // Do not allow copies of Logger by keeping the operators private
        //Logger(const Logger&);
        Logger& operator=(const Logger&);
};
#endif
