#include "thermal_profile.h"

#define SD_CS 53         // Chip select pin for sd card
#define DEBUG false
#define MIN_2_MILLIS 60000
#define MILLIS_2_MIN 0.00001666666666667
#define TEMP_THRESOLD 10 // (deg)


/// @brief Intialise thermal profile by reading in and parsing profile from temp.pro
/// @param None
/// @return bool if loading profile is succesful or not
bool ThermalProfile::init(void)
{

    Serial.println(F("ThermProf: Fetching profile"));

    // Setup sd card
    if (!SD.begin(SD_CS)) {
        Serial.println(F("ThermProf: SD card init fail"));
    }

    // Open the file on the sd card.  We always expect the profile to be the same name and < 8 char
    if (!SD.exists("temp.pro")) {
      Serial.println(F("ERROR: no profile file"));
      return false;
    }
    Serial.println(F("Profile file found"));

    File therm_file = SD.open("temp.pro");
    if (!therm_file) {
        Serial.println(F("ERROR: can't open profile"));
        return false;
    }

    // Use an enum to keep track of the data positions being read in
    enum DataPos {
        DURATION,
        TEMPERATURE
    };
    DataPos var_num = DURATION;

    bool have_header = false;
    bool block_floats = false;
    char buffer[6];
    memset(buffer, '\0', sizeof(buffer));
    uint8_t buffer_pos = 0;
    uint8_t index = 0;

    // Parse the data from the text file
    while (therm_file.available()) {

        // Safe guard the buffer
        if (buffer_pos >= sizeof(buffer)) {
          Serial.println(F("ERROR: Exceeded buffer size"));
          goto reset;
        }

        // safe guard the data arrays
        if (index >= MAX_PROFILE_POINTS-1) {
          break;
        }

        char c = therm_file.read();

        if (DEBUG) {
          Serial.println(c);
        }

        // Get past the header
        if (!have_header) {
            if ((c == '\n') || (c == '\r')) {
                have_header = true;
                if (DEBUG) {
                    Serial.println("Got the header");
                }
            }
            continue;
        }

        // If we got this far we are ready to parse the data
        if ((c == ',') || (c == '\n') || (c == '\r')) {

          switch (var_num) {

              case DURATION:
                  _durations[index] = to_int(buffer, sizeof(buffer));

                  if (DEBUG) {
                      Serial.println("Dur:");
                  }
                  var_num = TEMPERATURE;
                  break;

              case TEMPERATURE:
                  _temps[index] = to_int(buffer, sizeof(buffer));

                  // We only advance the index once we have completed a row
                  index++;

                  if (DEBUG) {
                      Serial.println("Temp:");
                  }
                  var_num = DURATION;
                  break;

              default:
                  Serial.println(F("ERROR: Data parsing position went wrong"));
          }

          if (DEBUG) {
              // We reached the end of the number, print the full buffer
              Serial.println(buffer);
          }

          // Clear buffer
          goto reset;
        }

        // We should have recieved a number
        if (isDigit(c) && !block_floats) {
            // stuff the data into the buffer
            buffer[buffer_pos] = c;
            buffer_pos++;
            continue;

        } else if (c == '.') {
            // Attempt to guard against floating points being accidently input and ignore all subsequent chars
            block_floats = true;
            continue;

        } else if ((c == '+') || (c == '-')) {
            // discard signs
            continue;
        }

        // We have read something that is not a number. Disgard the number to this point.
        if (isalpha(c)) {
            Serial.println(F("ERROR: Read alpha char"));
            goto reset;
        }

        reset:
            memset(buffer, '\0', sizeof(buffer));
            buffer_pos = 0;
            block_floats = false;

    }

    // close the file
    therm_file.close();

    // debug
    if (DEBUG) {
        Serial.println();
        Serial.println(F("data table:"));
        for (uint8_t i = 0; i < MAX_PROFILE_POINTS; i++) {
            Serial.print(_durations[i]);
            Serial.print(" .. ");
            Serial.println(_temps[i]);
        }
    }

    // Clearly init the starting position of the profile
    _profile_position = 0;

    // Succesfully setup the thermal profile
    return true;
}


/// @brief Parses a char array into an int16_t
/// @param c Char array to be converted to int16_t
/// @param n Size of char array
/// @return int16_t numerical value
int16_t ThermalProfile::to_int(char *c, uint8_t n)
{
    int16_t value = 0;
    for (uint8_t i = 0; i < n; i++) {
        if (isdigit(c[i])) {
            value *= 10;
            value += c[i] - '0';
        }
    }
    return value;
}


/// @brief Accessor function to provide desired temperatures from the uploaded profile. The desired values are time dependant only and come from the thermal profile provided. The returned values make no assumptions about what the machine is capible of, they are desired values only.
/// @param none
/// @return int16_t, next desired temperature
int16_t ThermalProfile::get_desired_temp(void)
{

    unsigned long now_ms = millis();

    // Safe guard against running out of profile
    if (_profile_position >= MAX_PROFILE_POINTS) {
        // TODO: Flag this as profile complete
        // Shutdown the heater
        return 0;
    }

    // Starting value
    if (_profile_position == 0) {
        _last_temp_target = _temps[0];
        _last_profile_step_ms = now_ms;
        _profile_position++;
        return _temps[0];
    }

    // See if we have waited the pre-requisit duration
    if ((now_ms - _last_profile_step_ms) >= (_durations[_profile_position]*MIN_2_MILLIS)) {
        // Move on to the next step in the profile
        _last_temp_target = _temps[_profile_position];
        _profile_position++;
        _last_profile_step_ms = now_ms;
    }

    // Calculate the temperature rate in degrees per min. Don't allow faster changes than per 1 min.
    float temp_rate_per_min = (_temps[_profile_position] - _last_temp_target) / max(_durations[_profile_position], 1);

    int16_t new_temp = _last_temp_target + (temp_rate_per_min * static_cast<float>((now_ms - _last_profile_step_ms) * MILLIS_2_MIN));

    // Protect against overshoots by constraining to the target
    if (temp_rate_per_min >= 0) {
        // Climbing temp target
        new_temp = min(max(new_temp, _last_temp_target), _temps[_profile_position]);
    } else {
        // Descending temp target
        new_temp = min(max(new_temp, _temps[_profile_position]), _last_temp_target);
    }

    return new_temp;
}