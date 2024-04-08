#ifndef PID_H
#define PID_H

typedef struct pid {
    float kp;
    float ki;
    float kd;
    float setpoint;
    float min_output;
    float max_output;
    float output;
    float previous_value;
    float integral_value;
} pid;

pid pid_new(float min, float max);
float pid_process(pid *pid, float value);
void pid_reset(pid *pid);

#endif
