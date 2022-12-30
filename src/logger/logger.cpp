#include "logger.h"


#define SD_CS 53                       // Chip select pin for sd card
#define FILE_LIMIT (uint16_t)1000      // Max number of files on sd
#define LOGGING_DT 500

#define ERROR_REPEAT 1000

// Constructor


void Logger::init(void)
{

    Serial.println("Setup SD card logging");

    // Setup sd card logging
    if (!SD.begin(SD_CS)) {
        raise_logger_error(Status::SD_INIT_FAILED);
    }

    // Create the file on the sd card
    bool file_created = false;
    char fileName[13];
    for (int n = 0; n < FILE_LIMIT; n++) {
        sprintf(fileName, "SDlog%.3d.TXT", n);
        if (!SD.exists(fileName)) {
            _sd_file = SD.open(fileName, FILE_WRITE);
        if (_sd_file) {
            _sd_file.println(F("Time(ms),DTemp(degC),TTemp(degC),MTemp(degC)"));
            _sd_file.flush();
            file_created = true;
        }
        break;
        }
    }

  // Ensure we created a file
  if (!file_created) {
    raise_logger_error(Status::SD_FILE_START_ERROR);
  }
}

void Logger::write(uint16_t data)
{
    unsigned long now_ms = millis();

  if (now_ms - _last_update_ms > LOGGING_DT) {

    // SD card Logging
    // Create a string buffer for float to string conversion before writting to sd card
    char buffer[39]; // Largest char array is time ms

    // time
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    sprintf(buffer, "%lu,", now_ms);
    _sd_file.print(buffer);

    // desired temperature
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    sprintf(buffer, "%i,", data);
    _sd_file.print(buffer);

    // flow rate
    // memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    // dtostrf(float(data), 20, 3, buffer);
    // _sd_file.print(buffer);

    // End of line
    _sd_file.print(F("\n"));

    _sd_file.flush();

    _last_update_ms = now_ms;
  }

}


void Logger::raise_logger_error(Status err)
{
  unsigned long now = millis();
  while (true) {

    if (now - _last_update_ms > ERROR_REPEAT) {

        // Spam reason to serial
        switch (err) {
            case SD_INIT_FAILED:
                Serial.println(F("SD card init failed"));
                break;

            case SD_FILE_START_ERROR:
                Serial.println(F("Falied to create file on sd"));
                break;

            default:
                Serial.println(F("Unknown crit error"));
                break; 
        }

        // We are hanging the script here so we can reuse _last_update_ms
        _last_update_ms = now;
    }
  }
}