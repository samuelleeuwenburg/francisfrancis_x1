#include <stdio.h>
#include <assert.h>
#include "pid.h"

void check(pid *pid, float input, float expecting) {
    pid_process(pid, input);

    printf(
           "input = %f\toutput = %f\texpected %f\n",
           input,
           pid->output,
           expecting
           );

    assert(pid->output == expecting);
}

void test_p() {
    pid pid = pid_new(0.0, 100.0);

    printf("setpoint: 10.0\tkp: 2.0:\n");
    pid.setpoint = 10.0;
    pid.kp = 2.0;

    check(&pid, 0.0, 20.0);
    check(&pid, 5.0, 10.0);
    check(&pid, 8.0, 4.0);
    check(&pid, 9.0, 2.0);
    check(&pid, 10.0, 0.0);
}

void test_i() {
    pid pid = pid_new(0.0, 1000.0);
    printf("setpoint: 20.0\tki: 2.0:\n");
    pid.setpoint = 20.0;
    pid.ki = 2.0;

    check(&pid, 0.0, 40.0);
    check(&pid, 0.0, 80.0);
    check(&pid, 10.0, 100.0);
    check(&pid, 15.0, 110.0);
    check(&pid, 30.0, 90.0);
    check(&pid, 50.0, 30.0);
}

void test_d() {
    pid pid = pid_new(-100.0, 100.0);
    printf("setpoint: 40.0\tkd: 6.0:\n");
    pid.setpoint = 40.0;
    pid.kd = 6.0;

    check(&pid, 0.0, 0.0);
    check(&pid, 5.0, -30.0);
    check(&pid, 10.0, -30.0);
    check(&pid, 20.0, -60.0);
}

void test_min_max() {
    pid pid = pid_new(-100.0, 100.0);

    printf("setpoint: 1.0\tki: 10.0:\tmax: 100.0\n");
    pid.setpoint = 1.0;
    pid.ki = 40.0;
    pid.max_output = 100.0;

    check(&pid, 0.0, 40.0);
    check(&pid, 0.0, 80.0);
    check(&pid, 0.0, 100.0);
    check(&pid, 0.0, 100.0);
    check(&pid, 2.0, 100.0);
    check(&pid, 2.0, 80.0);
    check(&pid, 2.0, 40.0);
    check(&pid, 2.0, 0.0);
    check(&pid, 2.0, -40.0);
    check(&pid, 2.0, -80.0);
    check(&pid, 2.0, -100.0);
    check(&pid, 2.0, -100.0);
    check(&pid, 2.0, -100.0);
}

int main() {
    printf("PID sanity check...\n");

    printf("\ntesting P\n");
    test_p();

    printf("\ntesting I\n");
    test_i();

    printf("\ntesting D\n");
    test_d();

    printf("\ntesting min/max\n");
    test_min_max();

    printf("\n\033[32mall good! \033[0m \n");
    return 0;
}
