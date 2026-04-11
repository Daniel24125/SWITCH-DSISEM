#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// BCM2711 Register Base Addresses
#define RPI4_PERIPH_BASE   0xFE000000
#define RPI4_GPIO_BASE     (RPI4_PERIPH_BASE + 0x200000)
#define RPI4_PWM0_BASE     (RPI4_GPIO_BASE + 0xC000)
#define RPI4_CLK_BASE      (RPI4_PERIPH_BASE + 0x101000)

// Clock Manager Offsets (relative to RPI4_CLK_BASE)
#define CM_PWMCTL          (0xA0 / 4)
#define CM_PWMDIV          (0xA4 / 4)
#define CM_PASSWORD        0x5A000000

// PWM Register Structure
typedef struct {
    uint32_t CONTROL;
    uint32_t STATUS;
    uint32_t DMAC[2];
    uint32_t CHN0_RANGE; 
    uint32_t CHN0_DATA;  
    uint32_t FIF1[2];    
    uint32_t CHN1_RANGE;
    uint32_t CHN1_DATA;
} bcm2711_pwm_registers_t;

// GPIO Register Structure
typedef struct {
    uint32_t GPFSEL[6];
    uint32_t Reserved0;
    uint32_t GPSET[2];
    uint32_t Reserved1;
    uint32_t GPCLR[2];
    uint32_t Reserved2;
    uint32_t GPLEV[2];
} bcm2711_gpio_registers_t;

// Function to set GPIO mode
static inline void set_gpio_function(volatile bcm2711_gpio_registers_t *gpio_regs, uint8_t pin, uint8_t func) {
    int reg_idx = pin / 10;
    int bit_offset = (pin % 10) * 3;
    
    uint32_t val = gpio_regs->GPFSEL[reg_idx];
    val &= ~(7 << bit_offset);
    val |= (func << bit_offset);
    gpio_regs->GPFSEL[reg_idx] = val;
}

// Safe Clock Initialization
static inline int setup_pwm_clock(volatile uint32_t *clk_regs, uint32_t divider) {
    // 1. Stop clock
    clk_regs[CM_PWMCTL] = CM_PASSWORD | 0x01; // Stop, keep OSC source
    
    // 2. Wait for BUSY to be 0 with a timeout
    int timeout = 1000;
    while ((clk_regs[CM_PWMCTL] & 0x80) && timeout--) {
        usleep(10);
    }
    
    if (timeout <= 0) {
        fprintf(stderr, "Warning: Clock busy bit timed out. Attempting to force stop.\n");
        clk_regs[CM_PWMCTL] = CM_PASSWORD | 0x20; // Use KILL bit
        usleep(100);
    }

    // 3. Set Divider
    clk_regs[CM_PWMDIV] = CM_PASSWORD | (divider << 12);

    // 4. Re-enable clock with Source 1 (OSC)
    clk_regs[CM_PWMCTL] = CM_PASSWORD | 0x11;
    
    return 0;
}

// PWM Channel 0 Configuration
static inline void setup_pwm_channel0(volatile bcm2711_pwm_registers_t *pwm_regs, uint32_t range, uint32_t data) {
    pwm_regs->CONTROL = 0; // Disable
    usleep(1000);
    
    pwm_regs->CHN0_RANGE = range;
    pwm_regs->CHN0_DATA = data;
    
    // Enable Channel 0 in Mark/Space mode
    // MSEN1 = bit 7, PWEN1 = bit 0
    pwm_regs->CONTROL = (1 << 7) | (1 << 0);
}

#endif
