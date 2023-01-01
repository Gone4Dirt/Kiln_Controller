#include "./src/logger/logger.h"
#include "./src/thermal_profile/thermal_profile.h"
#include "./src/temp_measure/temp_measure.h"
#include "./src/screen/screen.h"
#include "./src/pid/pid.h"



Logger _logger;
#define LOGGER_UPDATE_MS 1000
unsigned long _last_log_update_ms;

ThermalProfile _profile;
int16_t _desired_temp;

TempMeasure _mea_temp;
float _measured_temp;

Screen _display;
#define DISPLAY_UPDATE_MS 2500
unsigned long _last_display_update_ms;

PID _pid;
#define DT 1000 // (ms) 1 Hz running rate
unsigned long _last_ctrl_update_ms;
#define KP 1.5
#define KI 0.1
#define KD 0.01
#define IMAX 1.0

#define SSR_PIN 42
#define INDICATOR_LED_PIN 44
#define MIN_SWITCH_TIME 100 //(ms)
uint16_t _window_ms;
float _output;

bool _error_state;

void setup() {

    Serial.begin(9600);

    _error_state = false;

    // Setup display
    _display.init();

    // Read the thermal profile
    if (!_profile.init()){
        // TODO: add led/screen to tell user of the error
        Serial.println(F("ERROR caught"));
    }

    // Init logger after we have loaded the thermal profile to avoid conflicts with the sd card read/write
    _logger.init();

    // init temperature measurement
    _mea_temp.init(&_logger);

    // init pid controller
    _pid.init(&_logger, KP, KI, KD, IMAX);

    // Set the SSR control pin
    pinMode(SSR_PIN, OUTPUT);
    pinMode(INDICATOR_LED_PIN, OUTPUT);
    _window_ms = 0;
    _output = 0.0;

    // init last time
    _last_display_update_ms = 0;
    _last_log_update_ms = 0; // we want the logging to start right away
}


void loop()
{
    if (millis() - _last_ctrl_update_ms >= DT) {
        // update the desired temperature
        _desired_temp = _profile.get_desired_temp();
        _logger.log_therm_profile(millis(), _desired_temp);

        // update the measured temp
        if (!_mea_temp.update()) {
          // Flag the error on the screen
          _display.error();

          // Set error state to shut down kiln
          _error_state = true;

          // TODO: shut down kiln, we have a faulty temperature measurement
            Serial.println(F("ERROR caught"));
        }

        _measured_temp = _mea_temp.get_measured_temp();

        // --- Control ---
        // Keep running at constant speed, it should keep the pid controller happier
        // There is a speed limit to how fast we can run the SSR though

        if (millis() - _last_ctrl_update_ms >= DT) {
            _output = _pid.calc(_measured_temp, _desired_temp);

            _window_ms = min(max((_output * DT), 0), DT);
            // the SSR has a minimum time required to switch on.
            // we recognise that here by forcing the window time
            if (_window_ms < MIN_SWITCH_TIME) {
              _window_ms = 0;
            }

            _logger.log_output(_output, _window_ms);
            _last_ctrl_update_ms = millis();
        }
    }

    // Set the SSR control pin
    if (!_error_state &&  (millis() - _last_ctrl_update_ms <= _window_ms)) {
        // Switch heating element on
        digitalWrite(SSR_PIN, HIGH);
        digitalWrite(INDICATOR_LED_PIN, HIGH);

    } else {
        // Switch off heating element
        digitalWrite(SSR_PIN, LOW);
        digitalWrite(INDICATOR_LED_PIN, LOW);
    }

    // Logging
    if (millis() - _last_log_update_ms >= LOGGER_UPDATE_MS) {
        _logger.write();
        _last_log_update_ms = millis();
    }

    // Display
    if (millis() - _last_display_update_ms >= DISPLAY_UPDATE_MS) {
        _display.update(_desired_temp, static_cast<int16_t>(_measured_temp));
        _last_display_update_ms = millis();
    }

}
