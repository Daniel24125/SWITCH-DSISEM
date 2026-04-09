#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "setGPIO.h"

#define LED_PIN 12
#define RPI4_PERIPH_BASE 0xFE000000
#define RPI4_GPIO_BASE (RPI4_PERIPH_BASE + 0x200000)
#define ADDRESS_RANGE (4 * 1024) // 4KB memory page size

// GPIO Modes
#define GPIO_MODE_IN  0
#define GPIO_MODE_OUT 1

// GPIO Levels
// #define GPIO_LEVEL_LOW  0
// #define GPIO_LEVEL_HIGH 1

// typedef struct {
//     uint32_t GPFSEL[7]; /*!< GPIO Function Select */
//     uint32_t GPSET[3];  /*!< GPIO Pin Output Set */
//     uint32_t GPCLR[3];  /*!< GPIO Pin Output Clear */
//     uint32_t GPLEV[3];  /*!< GPIO Pin Level */
//     uint32_t GPEDS[3];  /*!< GPIO Pin Event Detect Status */
//     uint32_t GPREN[3];  /*!< GPIO Pin Rising Edge Detect Enable */
//     uint32_t GPFEN[3];  /*!< GPIO Pin Falling Edge Detect Enable */
//     uint32_t GPHEN[3];  /*!< GPIO Pin High Detect Enable */
//     uint32_t GPLEN[3];  /*!< GPIO Pin Low Detect Enable */
//     uint32_t GPAREN[3]; /*!< GPIO Pin Async. Rising Edge Detect */
//     uint32_t GPAFEN[3]; /*!< GPIO Pin Async. Falling Edge Detect */
//     uint32_t GPPUD;     /*!< GPIO Pin Pull-up/down Enable */
//     uint32_t GPPUDCLK[3];/*!< GPIO Pin Pull-up/down Enable Clock */
// } bcm2837_gpio_registers_t; 

/**
 * @brief Maps the GPIO registers to the process's virtual memory space.
 * 
 * @return Pointer to the mapped GPIO registers, or NULL on failure.
 */
bcm2837_gpio_registers_t* map_gpio_registers(void) {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Error opening /dev/mem");
        return NULL;
    }

    void *gpio_map = mmap(
        NULL,
        ADDRESS_RANGE,
        PROT_READ | PROT_WRITE, 
        MAP_SHARED,
        mem_fd,
        RPI4_GPIO_BASE 
    );

    close(mem_fd); // FD is no longer needed after mapping

    if (gpio_map == MAP_FAILED) {
        perror("Error mapping GPIO memory");
        return NULL;
    }

    return (bcm2837_gpio_registers_t *)gpio_map;
}

/**
 * @brief Unmaps the GPIO registers.
 */
void unmap_gpio_registers(bcm2837_gpio_registers_t *regs) {
    if (regs) {
        munmap((void *)regs, ADDRESS_RANGE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <0|1>\n", argv[0]);
        printf("  0: Turn LED OFF\n");
        printf("  1: Turn LED ON\n");
        return EXIT_FAILURE;
    }

    int state = atoi(argv[1]);
    if (state != 0 && state != 1) {
        printf("Error: Invalid state '%s'. Use 0 (OFF) or 1 (ON).\n", argv[1]);
        return EXIT_FAILURE;
    }

    // 1. Map GPIO
    bcm2837_gpio_registers_t *gpio_regs = map_gpio_registers();
    if (!gpio_regs) {
        fprintf(stderr, "Error: Could not map GPIO. Are you running as root (sudo)?\n");
        return EXIT_FAILURE;
    }

    // 2. Setup pin as Output
    set_gpio_mode(gpio_regs, LED_PIN, GPIO_MODE_OUT);

    // 3. Set LED state
    write_gpio(gpio_regs, LED_PIN, state);
    printf("LED %d is now %s!\n", LED_PIN, (state == 1) ? "ON" : "OFF");

    // 4. Cleanup
    unmap_gpio_registers(gpio_regs);

    return EXIT_SUCCESS;
}