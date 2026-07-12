#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Pin Definitions
#define PIN_STB 13
#define PIN_CLK 15
#define PIN_DIO 14

// TM1638 Commands
#define CMD_DATA 0x40
#define CMD_CTRL 0x80
#define CMD_ADDR 0xC0

// Font map for digits 0-9 (7-segment layouts)
const uint8_t FONT_MAP[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                            0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// Send a single byte over the 3-wire serial interface
void tm1638_send_byte(uint8_t byte) {
  gpio_set_dir(PIN_DIO, GPIO_OUT);
  for (int i = 0; i < 8; i++) {
    gpio_put(PIN_CLK, 0);
    sleep_us(1); // Necessary timing anchor for 150MHz clocks
    gpio_put(PIN_DIO, byte & 1);
    byte >>= 1;
    sleep_us(1);
    gpio_put(PIN_CLK, 1);
    sleep_us(1);
  }
}

// Read a single byte from the TM1638
uint8_t tm1638_read_byte() {
  uint8_t byte = 0;
  gpio_set_dir(PIN_DIO, GPIO_IN);
  sleep_us(2); // Let the line settle after switching directions
  for (int i = 0; i < 8; i++) {
    gpio_put(PIN_CLK, 0);
    sleep_us(1); // Small delay for timing stability
    gpio_put(PIN_CLK, 1);
    sleep_us(1);
    if (gpio_get(PIN_DIO)) {
      byte |= (1 << i);
    }
  }
  return byte;
}

// Send a command to the TM1638
void tm1638_send_cmd(uint8_t cmd) {
  gpio_put(PIN_STB, 0);
  sleep_us(1);
  tm1638_send_byte(cmd);
  gpio_put(PIN_STB, 1);
  sleep_us(1);
}

// Update a specific register address with data
void tm1638_send_data(uint8_t addr, uint8_t data) {
  tm1638_send_cmd(CMD_DATA); // Set to write mode
  gpio_put(PIN_STB, 0);
  sleep_us(1);
  tm1638_send_byte(CMD_ADDR | addr);
  tm1638_send_byte(data);
  gpio_put(PIN_STB, 1);
  sleep_us(1);
}

// Initialize GPIO pins and the display
void tm1638_init() {
  gpio_init(PIN_STB);
  gpio_init(PIN_CLK);
  gpio_init(PIN_DIO);

  gpio_set_dir(PIN_STB, GPIO_OUT);
  gpio_set_dir(PIN_CLK, GPIO_OUT);

  gpio_put(PIN_STB, 1);
  gpio_put(PIN_CLK, 1);

  // Turn on display, set brightness
  // 0x88 is ON, lower 3 bits 0-7 control brightness
  tm1638_send_cmd(CMD_CTRL | 0x08 | 2);

  // Clear all registers (16 bytes)
  for (uint8_t i = 0; i < 16; i++) {
    tm1638_send_data(i, 0x00);
  }
}

// Read the state of all 8 buttons
uint8_t tm1638_read_keys() {
  uint8_t keys = 0;
  gpio_put(PIN_STB, 0);
  sleep_us(1);
  tm1638_send_byte(0x42); // Send data-read instruction

  // Corrected array allocation initialization
  uint8_t segments[4] = {0, 0, 0, 0};
  for (int i = 0; i < 4; i++) {
    segments[i] = tm1638_read_byte();
  }
  gpio_put(PIN_STB, 1);
  sleep_us(1);

  // Map the incoming byte matrix to a single 8-bit value
  for (int i = 0; i < 4; i++) {
    if (segments[i] & 0x01) {
      keys |= (1 << i);
    }
    if (segments[i] & 0x10) {
      keys |= (1 << (i + 4));
    }
  }
  return keys;
}

// Set individual LEDs (8-bit mask: bit 0 controls LED 1, etc.)
void tm1638_set_leds(uint8_t mask) {
  for (int i = 0; i < 8; i++) {
    // LED addresses are at odd positions (1, 3, 5, 7, 9, 11, 13, 15)
    tm1638_send_data((i * 2) + 1, (mask & (1 << i)) ? 1 : 0);
  }
}

int main() {
  stdio_init_all();
  // Initialize the CYW43 architecture
  if (cyw43_arch_init()) {
    return -1;
  }

  tm1638_init();

  uint32_t counter = 0;

  while (true) {
    // Turn the LED on at pico board
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(100);

    // Turn the LED off at pico board
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(100);

    // Display a changing number across the digits
    uint32_t temp = counter;
    for (int i = 7; i >= 0; i--) {
      uint8_t digit = temp % 10;
      // Digit displays are at even addresses (0, 2, 4, 6, 8, 10, 12, 14)
      tm1638_send_data(i * 2, FONT_MAP[digit]);
      temp /= 10;
    }

    //   7   6   5   4   3   2   1   0
    //  ()  ()  ()  ()  ()  ()  ()  ()  LED
    //
    //
    //   7   6   5   4   3   2   1   0
    // [O] [O] [O] [O] [O] [O] [O] [O] BUTTONS
    //
    // Read which buttons are pressed
    uint8_t pressed_buttons = tm1638_read_keys();
    // Tun ON respective LED
    tm1638_set_leds(pressed_buttons);

    counter++;
    sleep_ms(1);
  }
  return 0;
}
