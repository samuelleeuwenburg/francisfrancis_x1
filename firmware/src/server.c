#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "server.h"
#include "heater.h"
#include "max31855.h"

#define TCP_PORT 80

int server_init() {
    // Wifi setup
    if (cyw43_arch_init()) {
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    return 0;
}

void server_deinit() {
    cyw43_arch_deinit();
}

int server_try_connect() {
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0) {
      // Disable onboard LED if we can't connect
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

      return 1;
    }

    // Enable onboard LED once we are connected
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    return 0;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    server_state *state = (server_state*)arg;
    pid *pid = heater_active_pid(state->heater);

    char buffer[256] = "";

    char headers[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Connection: Close\r\n"
        "\r\n";

    snprintf(
	     buffer,
	     sizeof(buffer),
	     "%s{"
             "\"thermocouple\": %.2f,"
             "\"reference\": %.2f,"
             "\"duty_cycle\": %.2f,"
             "\"setpoint\": %.2f,"
             "\"errors\": %u"
             "}",
	     headers,
	     state->thermocouple->temperature,
	     state->thermocouple->reference_temperature,
	     state->heater->duty_cycle,
	     pid->setpoint,
	     state->thermocouple->errors
	     );

    for (int i = 0; i < sizeof(buffer); i++) {
	// Replace any null terminators with whitespace instead
	if (buffer[i] == '\0') {
	    buffer[i] = ' ';
	}
    }

    err = tcp_write(client_pcb, buffer, sizeof(buffer), TCP_WRITE_FLAG_COPY);

    // Close the connection
    tcp_close(client_pcb);

    if (err != ERR_OK) {
        return err;
    }

    return ERR_OK;
}

int server_setup(server_state *state) {
    // Init server
    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        return 1;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err) {
        return 1;
    }

    struct tcp_pcb *server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!server_pcb) {
        if (pcb) {
            tcp_close(pcb);
        }
        return 1;
    }

    // Pass the state reference as arg
    tcp_arg(server_pcb, state);

    tcp_accept(server_pcb, tcp_server_accept);

    return 0;
}

void server_poll() {
    cyw43_arch_poll();
}

