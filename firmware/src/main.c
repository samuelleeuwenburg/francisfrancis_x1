#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include "max31855.h"
#include "heater.h"
#include "server.h"

int main() {
    stdio_init_all();
    server_init();

    max31855 thermocouple = max31855_new();
    heater heater = heater_new();
    server_state state = {
        .heater = &heater,
        .thermocouple = &thermocouple,
    };

    max31855_init(&thermocouple);
    heater_init(&heater);

    server_setup(&state);

    for (;;) {
        sleep_ms(500);

	server_try_connect();
        server_poll();

        max31855_read(&thermocouple);

	if (thermocouple.errors > 0) {
	  heater_set_duty_cycle(&heater, 0.0);
	  continue;
	}

        heater_read_mode(&heater);
        heater_process(&heater, thermocouple.temperature);
    }

    server_deinit();

    return 0;
}
