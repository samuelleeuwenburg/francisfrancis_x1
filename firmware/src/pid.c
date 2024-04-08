#include "pid.h"

pid pid_new(float min, float max) {
    pid pid = {
        .kp = 0.0,
        .ki = 0.0,
        .kd = 0.0,
        .setpoint = 0.0,
        .min_output = min,
        .max_output = max,
        .output = 0.0,
        .previous_value = 0.0,
        .integral_value = 0.0,
    };

    return pid;
}

float pid_process(pid *pid, float value) {
    float error = pid->setpoint - value;

    // Proportional gain
    float p = error * pid->kp;

    // Integral
    pid->integral_value += error * pid->ki;

    // Derivative
    float d = -(value - pid->previous_value) * pid->kd;
    pid->previous_value = value;

    // ∑
    pid->output = p + pid->integral_value + d;

    // Clamp
    if (pid->output < pid->min_output) {
        pid->output = pid->min_output;
    } else if (pid->output > pid->max_output) {
        pid->output = pid->max_output;
    }

    return pid->output;
}

void pid_reset(pid *pid) {
    pid->output = 0.0;
    pid->previous_value = 0.0;
    pid->integral_value = 0.0;
}
