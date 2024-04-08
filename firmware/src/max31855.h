#ifndef MAX31855_H
#define MAX31855_H

#include "pico/stdlib.h"

typedef struct max31855 {
    float temperature;
    float reference_temperature;
    // 0bXXXX: fault_bit, short_to_vcc, short_to_gnd, open_circuit
    uint8_t errors;
} max31855;

max31855 max31855_new();
void max31855_init(max31855 *m);
void max31855_read(max31855 *m); 

#endif
