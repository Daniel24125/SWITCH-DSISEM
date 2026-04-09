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
#define GPIO_MODE_ALT_0 4 //100 - PWM for GPIO12

typedef struct {
    uint32_t CONTROL;
    uint32_t STATUS;
    uint32_t DMAC[2];
    uint32_t CHN0_RANGE; //maximum value for PWM_CHANNEL0_DATA
    uint32_t CHN0_DATA; //Pulse Width for CHANNEL0 (from 0 to PWM_CHN0_RANGE)
    uint32_t FIF1[2]; // PWM FIFO Input
    uint32_t CHN1_RANGE;
    uint32_t CHN1_DATA;
} bmc2711_pwm_registers_t;


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

bmc2711_pwm_registers_t *map_pwm_registers(void){
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Error opening /dev/mem");
        return NULL;
    }

    void *pwm_map = mmap(
        NULL,
        sizeof(bmc2711_pwm_registers_t),
        PROT_READ | PROT_WRITE, 
        MAP_SHARED,
        mem_fd,
        RPI4_GPIO_BASE 
    );

    close(mem_fd); // FD is no longer needed after mapping

    if (pwm_map == MAP_FAILED) {
        perror("Error mapping GPIO memory");
        return NULL;
    }

    return (bmc2711_pwm_registers_t *)pwm_map;
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
    if (argc < 1) {
        printf("Usage: %s <1|255>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int state = atoi(argv[1]);
    if (state < 0 || state > 255) {
        printf("Error: Invalid state '%s'. Use between 0 and 255.\n", argv[1]);
        return EXIT_FAILURE;
    }

    // 1. Map GPIO
    bcm2837_gpio_registers_t *gpio_regs = map_gpio_registers();
    if (!gpio_regs) {
        fprintf(stderr, "Error: Could not map GPIO. Are you running as root (sudo)?\n");
        return EXIT_FAILURE;
    }

    // 2. Configure PWM
    bmc2711_pwm_registers_t *pwm_regs = map_pwm_registers();
    if(!pwm_regs){
        fprintf(stderr, "Error: Could not configure PWM\n");
        return EXIT_FAILURE;
    }

    // 2. Setup pin 12 as PWM Alt
    set_gpio_mode(gpio_regs, LED_PIN, GPIO_MODE_ALT_0);

    // 3. Set LED state
    write_gpio(gpio_regs, LED_PIN, state);
    printf("LED %d is now %s!\n", LED_PIN, (state == 1) ? "ON" : "OFF");

    // 4. Cleanup
    unmap_gpio_registers(gpio_regs);

    return EXIT_SUCCESS;
}