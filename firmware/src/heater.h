#ifndef HEATER_H
#define HEATER_H

#include "pico/types.h"
#include "hardware/pio.h"
#include "pid.h"

typedef enum mode {
  Espresso = 0,
  Steamer = 1
} mode;

typedef struct heater {
    pid espresso_pid;
    pid steamer_pid;
    float duty_cycle;
    mode mode;
    PIO pio;
    uint pio_sm;
} heater;

heater heater_new();
void heater_init(heater *h);

// Read and update the state of the temperature switch
void heater_read_mode(heater *h);

// Update the PID control with new temperature data
void heater_process(heater *h, float temperature);

void heater_set_duty_cycle(heater *h, float duty_cycle);

pid *heater_active_pid(heater *h);

#endif
