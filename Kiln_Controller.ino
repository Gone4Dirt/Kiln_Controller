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
    mea_temp.init();
    //_measured_temp = 0.0;

    // init last time
    _last_ms = millis();

}


void loop()
{

    unsigned long now_ms = millis();

    // update the desired temperature
    int16_t _desired_temp = _profile.get_desired_temp();
    // Serial.println(_desired_temp);

    // update the measured temp
    mea_temp.update();

    _logger.write(_desired_temp);

    _last_ms = now_ms;

    delay(1000);
}
