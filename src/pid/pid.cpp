/*
    PID controller that operates between 0 and 1
*/

#include "pid.h"

#define DEBUG false         // Easy enable/disable debugger for prints to serial console
#define MS_2_SEC 0.001

// Constructor
PID::PID(void)
{

    if (DEBUG) {
        Serial.println(F("PID constructor"));
    }

    // Explicitly set all gains and limits to zero on construction
    _kp = 0.0;
    _ki = 0.0;
    _kd = 0.0;
    _imax = 0.0;
    _last_err = 0.0;
    _ctrl_init = false;
}


void PID::init(Logger *ptr)
{
    if (DEBUG) {
        Serial.println(F("PID init"));
    }

    // Store the pointer to the logger object
    _logger = ptr;
}


void PID::init(Logger *ptr, float kp, float ki, float kd, float imax)
{
    if (DEBUG) {
        Serial.println(F("PID init"));
    }

    // set gains
    set_pid(kp, ki, kd);
    set_imax(imax);

    // Store the pointer to the logger object
    _logger = ptr;
}


/// @brief 
/// @param 
/// @return 
float PID::calc(float input, float setpoint)
{
    // use normalised error to keep controller in the -1 to 1 range
    float error = (setpoint - input) / max(setpoint, 1.0);

    // Smooth out steps in controller at start
    if (!_ctrl_init) {
        reset(error);
        _ctrl_init = true;
    }

    unsigned long now_ms = millis();

    float p = _kp*error;

    // Calculate time step size. We do not wan to assume this
    float dt = static_cast<float>(now_ms - _last_ms)*MS_2_SEC; // (s)
    dt = max(dt, 1e-4f); // Guard against dev by zero

    // Calculate the integrator contribution only if the controller has not been limited
    if (!_limit_flag) {
        _i += _ki * error * dt;
        _i = min(max(_i, 0.0), _imax);
    }
    // Serial.println(_i);

    float d = _kd * (error - _last_err) / dt;

    // normalised output
    float output = (p + _i + d);

    // Check if we should be limiting the controller
    if (output > 1) {
        output = 1.0;
        _limit_flag = true;

    } else if (output < 0) {
        output = 0.0;
        _limit_flag = true;

    } else {
        _limit_flag = false;
    }

    // log the contributions
    _logger->log_controller(now_ms, p, _i, d, _limit_flag);

    // update the history
    _last_ms = now_ms;
    _last_err = error;

    return output;
}


void PID::reset(float error)
{
    _last_err = error;
    _i = 0.0;
    _limit_flag == false;
    _last_ms = millis();
}


void PID::set_pid(float kp, float ki, float kd)
{
    _kp = kp;
    _ki = ki;
    _kd = kd;
}


void PID::set_kp(float kp)
{
    _kp = kp;
}


void PID::set_ki(float ki)
{
    _ki = ki;
}


void PID::set_kd(float kd)
{
    _kd = kd;
}


void PID::set_imax(float imax)
{
    _imax = imax;
}
