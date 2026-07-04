// examples/blink_rp2040/blink_simple.c
#include "pico/stdlib.h"

#define LED_DELAY_MS 250

// Initialize the GPIO for the LED
void pico_led_init(void) {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

// Turn the LED on or off
void pico_set_led(bool state) {
  gpio_put(PICO_DEFAULT_LED_PIN, state);
}

int main() {
  pico_led_init();
  while (true) {
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
  }
}
