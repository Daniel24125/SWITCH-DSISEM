#ifndef SETGPIO_H_
#define SETGPIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// GPIO Levels
#define GPIO_LEVEL_LOW  0
#define GPIO_LEVEL_HIGH 1

typedef struct {
    uint32_t GPFSEL[7]; /*!< GPIO Function Select */
    uint32_t GPSET[3];  /*!< GPIO Pin Output Set */
    uint32_t GPCLR[3];  /*!< GPIO Pin Output Clear */
    uint32_t GPLEV[3];  /*!< GPIO Pin Level */
    uint32_t GPEDS[3];  /*!< GPIO Pin Event Detect Status */
    uint32_t GPREN[3];  /*!< GPIO Pin Rising Edge Detect Enable */
    uint32_t GPFEN[3];  /*!< GPIO Pin Falling Edge Detect Enable */
    uint32_t GPHEN[3];  /*!< GPIO Pin High Detect Enable */
    uint32_t GPLEN[3];  /*!< GPIO Pin Low Detect Enable */
    uint32_t GPAREN[3]; /*!< GPIO Pin Async. Rising Edge Detect */
    uint32_t GPAFEN[3]; /*!< GPIO Pin Async. Falling Edge Detect */
    uint32_t GPPUD;     /*!< GPIO Pin Pull-up/down Enable */
    uint32_t GPPUDCLK[3];/*!< GPIO Pin Pull-up/down Enable Clock */
} bcm2837_gpio_registers_t;

/**
 * @brief Sets the function mode for a specific GPIO pin.
 */
void set_gpio_mode(bcm2837_gpio_registers_t *regs, uint8_t pin, uint8_t mode) {
    uint32_t bank = pin / 10;
    uint32_t bit_pos = (pin % 10) * 3;

    // Clear the current 3 bits for this pin, then set the new mode
    regs->GPFSEL[bank] &= ~(0b111 << bit_pos);
    regs->GPFSEL[bank] |= (mode << bit_pos);
}

/**
 * @brief Writes a digital level (High or Low) to a GPIO pin.
 */
void write_gpio(bcm2837_gpio_registers_t *regs, uint8_t pin, uint8_t level) {
    uint32_t bank = pin / 32;
    uint32_t bit_pos = (pin % 32);

    if (level == GPIO_LEVEL_HIGH) {
        regs->GPSET[bank] = (1 << bit_pos);
    } else {
        regs->GPCLR[bank] = (1 << bit_pos);
    }
}

#endif