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
            /* DEFINITIONS
              LTime(ms) = Log time, time that the log was written,
              PTime(ms) = Thermal profile time, last time profile was updated,
              PT(degC)  = Profile temperatire, the last updated profile temperature (desired temp),
              MTime(ms) = Measurement time, the last time that the measurement was updated,
              BT(degC)  = Board temperature, the last updated board temperature (cold junction),
              TT(degC)  = Thermocouple temperature, the last updated thermocouple temperature,
            */
            _sd_file.println(F("LTime(ms),PTime(ms),PT(degC),MTime(ms),BT(degC),TT(degC)"));
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

/// @brief Write latest values to SD card log
/// @param None
void Logger::write(void)
{
    unsigned long now_ms = millis();

    // SD card Logging
    // Create a string buffer for float to string conversion before writting to sd card
    char buffer[39]; // Largest char array is time ms

    // LTime(ms)
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    sprintf(buffer, "%lu,", now_ms);
    _sd_file.print(buffer);

    // PTime(ms)
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    sprintf(buffer, "%lu,", _therm_prof_time_ms);
    _sd_file.print(buffer);

    // PT(degC)
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    sprintf(buffer, "%i,", _desired_temp);
    _sd_file.print(buffer);

    // MTime(ms)
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    sprintf(buffer, "%lu,", _measure_time_ms);
    _sd_file.print(buffer);

    // BT(degC)
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    dtostrf(_board_temp, 8, 3, buffer);
    _sd_file.print(buffer);
    _sd_file.print(",");

    // TT(degC)
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer
    dtostrf(_thermocouple_temp, 8, 3, buffer);
    _sd_file.print(buffer);

    // End of line
    _sd_file.print(F("\n"));

    _sd_file.flush();

    _last_update_ms = now_ms;

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


void Logger::log_measurement(unsigned long time_ms, float bt, float tt, uint8_t flt)
{
    _measure_time_ms = time_ms;
    _board_temp = bt;
    _thermocouple_temp = tt;
    _fault = flt;
}


void Logger::log_therm_profile(unsigned long time_ms, int16_t dest)
{
    _therm_prof_time_ms = time_ms;
    _desired_temp = dest;
}
