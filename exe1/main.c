#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int flag_LED = 0;
volatile int g_timer_0 = 0;

bool timer_0_callback([[maybe_unused]] repeating_timer_t *rt) {
    g_timer_0 = 1;
    flag_LED = !flag_LED;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R) {
        if (events & GPIO_IRQ_EDGE_FALL) { 
            flag_f_r = !flag_f_r;
        }
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    int timer_0_hz = 2;
    repeating_timer_t timer_0;

    if (!add_repeating_timer_us(1000000 / timer_0_hz, timer_0_callback, NULL, &timer_0)) {
        printf("Failed to add timer\n");
    }

    while (true) {
        if (flag_f_r) {
            gpio_put(LED_PIN_R, flag_LED);
        }
    }
}
