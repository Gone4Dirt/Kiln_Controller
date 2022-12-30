#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

class Logger
{
    public:
        // void Logger(void);
        void init(void);

        void write(uint16_t data);

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

        // Functions
        void raise_logger_error(Status err);
};
#endif
