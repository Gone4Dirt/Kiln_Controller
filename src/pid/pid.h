#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include "../logger/logger.h"

class PID
{
    public:
        PID(void);

        void init(Logger *ptr);

        void init(Logger *ptr, float kp, float ki, float kd, float imax);

        float calc(float input, float setpoint);

        void reset(float error);

        void set_pid(float kp, float ki, float kd);

        void set_kp(float kp);

        void set_ki(float ki);

        void set_kd(float kd);

        void set_imax(float imax);

    private:

        float _kp;
        float _ki;
        float _kd;
        float _imax;
        float _last_err;
        float _i; // i-term for the controller

        // limiter used to prevent I term wind up
        bool _limit_flag;

        bool _ctrl_init;

        unsigned long _last_ms;

        Logger *_logger;

};
#endif
