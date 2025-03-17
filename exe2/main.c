#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int flag_led_r = 0;
volatile int flag_led_g = 0;

bool timer_r_callback(repeating_timer_t *rt) {
    flag_led_r = !flag_led_r;
    return true;
}

bool timer_g_callback(repeating_timer_t *rt) {
    flag_led_g = !flag_led_g;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = !flag_r;
        else if (gpio == BTN_PIN_G)
            flag_g = !flag_g;
    }
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_r_hz = 2;
    int timer_g_hz = 4;
    repeating_timer_t timer_r;
    repeating_timer_t timer_g;

    if (!add_repeating_timer_us(1000000 / timer_r_hz, timer_r_callback, NULL, &timer_r)) {
        printf("Failed to add timer\n");
    }

    if (!add_repeating_timer_us(1000000 / timer_g_hz, timer_g_callback, NULL, &timer_g)) {
        printf("Failed to add timer\n");
    }

    while (true) {

        if (flag_r) {
            gpio_put(LED_PIN_R, flag_led_r);
        }

        if (flag_g) {
            gpio_put(LED_PIN_G, flag_led_g);
        }
    }
}
