#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "heater.h"
#include "pwm.pio.h"

#define ESPRESSO_TEMP 93.0
#define STEAM_TEMP 130.0

// GPIO
#define HEATER_CONTROL_PIN 19
#define HEATER_MODE_PIN 20

// PIO
#define CLOCK_DIVIDER 512.0
#define MAX_PWM_VALUE 0xffff

heater heater_new() {
    pid espresso_pid = pid_new(0.0, 100.0);
    espresso_pid.kp = 3.2;
    espresso_pid.ki = 0.002;
    espresso_pid.setpoint = ESPRESSO_TEMP;

    pid steamer_pid = pid_new(0.0, 100.0);
    steamer_pid.kp = 3.8;
    steamer_pid.ki = 0.002;
    steamer_pid.setpoint = STEAM_TEMP;

    heater h = {
        .espresso_pid = espresso_pid,
        .steamer_pid = steamer_pid,
        .duty_cycle = 0.0,
        .mode = Espresso,
        .pio = pio0,
        .pio_sm = pio_claim_unused_sm(pio0, true),
    };

    return h;
}


static void update_duty_cycle(heater *h) {
    // Convert percentile duty cycle to 16 bit value
    int level = (h->duty_cycle / 100.0) * MAX_PWM_VALUE;
    // pio_sm_put_blocking(h->pio, h->pio_sm, level);
    pio_sm_put(h->pio, h->pio_sm, level);
}

void heater_init(heater *h) {
    // Mode input
    gpio_init(HEATER_MODE_PIN);
    gpio_set_dir(HEATER_MODE_PIN, GPIO_IN);

    // Init PWM PIO program
    uint offset = pio_add_program(h->pio, &pwm_program);
    pwm_program_init(h->pio, h->pio_sm, offset, HEATER_CONTROL_PIN);

    // Setup total cycle duration
    pio_sm_set_enabled(h->pio, h->pio_sm, false);
    pio_sm_put_blocking(h->pio, h->pio_sm, (1u << 16) - 1);
    pio_sm_exec(h->pio, h->pio_sm, pio_encode_pull(false, false));
    pio_sm_exec(h->pio, h->pio_sm, pio_encode_out(pio_isr, 32));
    pio_sm_set_clkdiv(h->pio, h->pio_sm, CLOCK_DIVIDER);
    pio_sm_set_enabled(h->pio, h->pio_sm, true);
}

pid *heater_active_pid(heater *h) {
  if (h->mode == Espresso) {
    return &h->espresso_pid;
  } else {
    return &h->steamer_pid;
  }
}

void heater_read_mode(heater *h) {
    mode new_mode = gpio_get(HEATER_MODE_PIN);

    // If the mode changed, reset the pids and update the mode
    if (new_mode != h->mode) {
        pid_reset(&h->espresso_pid);
        pid_reset(&h->steamer_pid);
	h->mode = new_mode;
    }
}

void heater_set_duty_cycle(heater *h, float duty_cycle) {
    h->duty_cycle = duty_cycle;
    update_duty_cycle(h);
}

void heater_process(heater *h, float temperature) {
    pid *pid = heater_active_pid(h);
    float duty_cycle = pid_process(pid, temperature);
    heater_set_duty_cycle(h, duty_cycle);
}
