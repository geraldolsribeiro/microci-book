// examples/blink_rp2350/blink_simple.c
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define LED_DELAY_MS 250

// Turn the LED on or off
void pico_set_led(bool state) {
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, state);
}

int main() {
  cyw43_arch_init(); // Initialize the CYW43 architecture
  while (true) {
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
  }
}
