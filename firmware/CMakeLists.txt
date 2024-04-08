cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_SDK_FETCH_FROM_GIT "https://github.com/raspberrypi/pico-sdk")
set(PICO_BOARD "pico_w")
set(CMAKE_BUILD_TYPE Debug)

set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(francisfrancisx1 C CXX ASM)

pico_sdk_init()

add_compile_options(-Wall
    -Wno-format # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    )

add_executable(francisfrancisx1)

target_compile_definitions(francisfrancisx1 PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    )

pico_generate_pio_header(francisfrancisx1 ${CMAKE_CURRENT_LIST_DIR}/src/pwm.pio)

target_include_directories(francisfrancisx1 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    )

target_sources(francisfrancisx1 PRIVATE
    src/main.c
    src/pid.c
    src/max31855.c
    src/heater.c
    src/server.c
    )

target_link_libraries(francisfrancisx1 PRIVATE
    pico_stdlib
    pico_cyw43_arch_lwip_poll
    hardware_spi
    hardware_pio
    )

pico_enable_stdio_uart(francisfrancisx1 1)

pico_add_extra_outputs(francisfrancisx1)
