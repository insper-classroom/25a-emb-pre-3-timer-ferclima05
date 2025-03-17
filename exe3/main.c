#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile bool flag_pressed = false;
volatile absolute_time_t press_time;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R) {
        if (events == GPIO_IRQ_EDGE_FALL) {
            flag_pressed = true;
            press_time = get_absolute_time();
        } else if (events == GPIO_IRQ_EDGE_RISE) {
            if (flag_pressed) {
                absolute_time_t release_time = get_absolute_time();
                int64_t duration = absolute_time_diff_us(press_time, release_time) / 1000; // Tempo em ms
                
                if (duration >= 500) {
                    static bool led_state = false;
                    led_state = !led_state;
                    gpio_put(LED_PIN_R, led_state);
                }
                flag_pressed = false;
            }
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, false);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        tight_loop_contents();
    }
}
