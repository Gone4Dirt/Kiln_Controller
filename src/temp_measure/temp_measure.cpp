#include "temp_measure.h"

#define DEBUG true         // Easy enable/disable debugger for prints to serial console

// Constructor
TempMeasure::TempMeasure(void){}


void TempMeasure::init(void)
{

    if (DEBUG) {
        Serial.println(F("Thermocouple constructor"));
    }

    // Init thermocouple
    _thermocouple.begin();

    // Type K thermocouple
    _thermocouple.setThermocoupleType(MAX31856_TCTYPE_K);

    if (DEBUG) {
        Serial.print(F("Thermocouple type: "));
        switch (_thermocouple.getThermocoupleType() ) {
            case MAX31856_TCTYPE_B: Serial.println("B Type"); break;
            case MAX31856_TCTYPE_E: Serial.println("E Type"); break;
            case MAX31856_TCTYPE_J: Serial.println("J Type"); break;
            case MAX31856_TCTYPE_K: Serial.println("K Type"); break;
            case MAX31856_TCTYPE_N: Serial.println("N Type"); break;
            case MAX31856_TCTYPE_R: Serial.println("R Type"); break;
            case MAX31856_TCTYPE_S: Serial.println("S Type"); break;
            case MAX31856_TCTYPE_T: Serial.println("T Type"); break;
            case MAX31856_VMODE_G8: Serial.println("Voltage x8 Gain mode"); break;
            case MAX31856_VMODE_G32: Serial.println("Voltage x8 Gain mode"); break;
            default: Serial.println("Unknown"); break;
        }
    }

    if (DEBUG) {
        Serial.println(F("BoardT,ThermoT"));
    }

    last_update_ms = millis();
}

bool TempMeasure::update(void)
{
    if (has_fault()) {
        return false;
    }

    // Update readings
    // board temp (cold junction)
    board_temp = _thermocouple.readCJTemperature();

    // Thermocouple
    therm_temp = _thermocouple.readThermocoupleTemperature();

    last_update_ms = millis();

    if (DEBUG) {
        Serial.print(board_temp);
        Serial.print(",");
        Serial.println(therm_temp);
    }

    return true;

}

bool TempMeasure::has_fault(void)
{
    // Check and print any faults
    uint8_t fault = _thermocouple.readFault();
    if (fault) {
        if (fault & MAX31856_FAULT_CJRANGE) Serial.println("Cold Junction Range Fault");
        if (fault & MAX31856_FAULT_TCRANGE) Serial.println("Thermocouple Range Fault");
        if (fault & MAX31856_FAULT_CJHIGH)  Serial.println("Cold Junction High Fault");
        if (fault & MAX31856_FAULT_CJLOW)   Serial.println("Cold Junction Low Fault");
        if (fault & MAX31856_FAULT_TCHIGH)  Serial.println("Thermocouple High Fault");
        if (fault & MAX31856_FAULT_TCLOW)   Serial.println("Thermocouple Low Fault");
        if (fault & MAX31856_FAULT_OVUV)    Serial.println("Over/Under Voltage Fault");
        if (fault & MAX31856_FAULT_OPEN)    Serial.println("Thermocouple Open Fault");
        return true;
    }
    return false;
}

