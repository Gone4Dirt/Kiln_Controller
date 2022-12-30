#include "./src/logger/logger.h"
#include "./src/thermal_profile/thermal_profile.h"

Logger _logger;
ThermalProfile _profile;

float _measured_temp;

unsigned long _last_ms;

void setup() {

    Serial.begin(9600);

    // Read the thermal profile
    if (!_profile.init()){
        // TODO: add led/screen to tell user of the error
        Serial.println(F("ERROR caught"));
    }

    _logger.init();

    // init temperature measurement
    _measured_temp = 0.0;

    // init last time
    _last_ms = millis();

}


void loop()
{

    unsigned long now_ms = millis();

    // update the desired temperature
    int16_t _desired_temp = _profile.get_desired_temp();

    Serial.println(_desired_temp);

    _logger.write(_desired_temp);

    _last_ms = now_ms;

    delay(1000);
}
