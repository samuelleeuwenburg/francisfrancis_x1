#ifndef SERVER_H
#define SERVER_H

#include "heater.h"
#include "max31855.h"

typedef struct server_state {
    heater *heater;
    max31855 *thermocouple;
} server_state;

int server_init();
void server_deinit();
int server_setup(server_state *state);
int server_try_connect();
void server_poll();

#endif
