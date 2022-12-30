#include "./src/logger/logger.h"
#include "./src/thermal_profile/thermal_profile.h"
#include "./src/temp_measure/temp_measure.h"

#define DT 500 // (ms) 2 Hz running rate

Logger _logger;
ThermalProfile _profile;
TempMeasure mea_temp;

float _measured_temp;

unsigned long _last_ms;

void setup() {

    Serial.begin(9600);

    // Read the thermal profile
    if (!_profile.init()){
        // TODO: add led/screen to tell user of the error
        Serial.println(F("ERROR caught"));
    }

    // Init logger after we have loaded the thermal profile to avoid conflicts with the sd card read/write
    _logger.init();

    // init temperature measurement
    mea_temp.init(&_logger);

    // init last time
    _last_ms = millis();

}


void loop()
{

    unsigned long start_ms = millis();

    // update the desired temperature
    int16_t _desired_temp = _profile.get_desired_temp();
    _logger.log_therm_profile(start_ms, _desired_temp);

    // update the measured temp
    if (!mea_temp.update()) {
      // TODO: add led/screen to tell user of the error
      // TODO: shut down kiln, we have a faulty temperature measurement
        Serial.println(F("ERROR caught"));
    }

    _logger.write();

    _last_ms = start_ms;

    // Keep running at constant speed
    uint16_t wait_ms = min(max(DT - (millis() - start_ms), 1), DT);
    delay(wait_ms);

}
