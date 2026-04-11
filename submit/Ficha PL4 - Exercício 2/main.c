#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "helperFunctions.h"

int main(int argc, char *argv[]) {
    // 1. Check command line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <duty_cycle (0-100)>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int duty_cycle = atoi(argv[1]);
    if (duty_cycle < 0 || duty_cycle > 100) {
        fprintf(stderr, "Duty cycle must be between 0 and 100.\n");
        return EXIT_FAILURE;
    }

    // 2. Open /dev/mem
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("Failed to open /dev/mem. Did you use sudo?");
        return EXIT_FAILURE;
    }

    // 3. Map Registers
    volatile bcm2711_gpio_registers_t *gpio_regs = (volatile bcm2711_gpio_registers_t *)mmap(
        NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, RPI4_GPIO_BASE
    );
    
    volatile bcm2711_pwm_registers_t *pwm_regs = (volatile bcm2711_pwm_registers_t *)mmap(
        NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, RPI4_PWM0_BASE
    );
    
    volatile uint32_t *clk_regs = (volatile uint32_t *)mmap(
        NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, RPI4_CLK_BASE
    );

    if (gpio_regs == MAP_FAILED || pwm_regs == MAP_FAILED || clk_regs == MAP_FAILED) {
        perror("mmap failed");
        close(mem_fd);
        return EXIT_FAILURE;
    }
    close(mem_fd);

    // 4. Configure GPIO 12 for PWM0 Channel 0 (Alt Function 0 = 4)
    set_gpio_function(gpio_regs, 12, 4);

    // 5. Configure PWM Clock (54MHz / 27 = 2MHz)
    printf("Configuring PWM clock...\n");
    setup_pwm_clock(clk_regs, 27);

    // 6. Configure PWM Channel 0 (Range 100 for 20kHz frequency @ 2MHz clock)
    printf("Configuring PWM channel 0 with duty cycle %d%%...\n", duty_cycle);
    setup_pwm_channel0(pwm_regs, 100, duty_cycle);

    printf("PWM Generator active on GPIO 12.\n");

    // Clean up
    munmap((void *)clk_regs, 4096);
    munmap((void *)pwm_regs, 4096);
    munmap((void *)gpio_regs, 4096);

    return EXIT_SUCCESS;
}