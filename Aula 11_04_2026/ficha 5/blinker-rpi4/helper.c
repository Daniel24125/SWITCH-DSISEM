#include "helper.h"

// Function to set GPIO mode
void set_gpio_function(volatile bcm2711_gpio_registers_t *gpio_regs, u8 pin, u8 func) {
    int reg_idx = pin / 10;
    int bit_offset = (pin % 10) * 3;
    
    u32 val = gpio_regs->GPFSEL[reg_idx];
    val &= ~(7 << bit_offset);
    val |= (func << bit_offset);
    gpio_regs->GPFSEL[reg_idx] = val;
}

// Safe Clock Initialization
int setup_pwm_clock(volatile u32 *clk_regs, u32 divider) {
    // 1. Stop clock
    clk_regs[CM_PWMCTL] = CM_PASSWORD | 0x01; // Stop, keep OSC source
    
    // 2. Wait for BUSY to be 0 with a timeout
    int timeout = 1000;
    while ((clk_regs[CM_PWMCTL] & 0x80) && timeout--) {
        udelay(10);
    }
    
    if (timeout <= 0) {
        printk(KERN_WARNING "Warning: Clock busy bit timed out. Attempting to force stop.\n");
        clk_regs[CM_PWMCTL] = CM_PASSWORD | 0x20; // Use KILL bit
        udelay(100);
    }

    // 3. Set Divider
    clk_regs[CM_PWMDIV] = CM_PASSWORD | (divider << 12);

    // 4. Re-enable clock with Source 1 (OSC)
    clk_regs[CM_PWMCTL] = CM_PASSWORD | 0x11;
    
    return 0;
}

// PWM Channel 0 Configuration
void setup_pwm_channel0(volatile bcm2711_pwm_registers_t *pwm_regs, u32 range, u32 data) {
    pwm_regs->CONTROL = 0; // Disable
    udelay(1000);
    
    pwm_regs->CHN0_RANGE = range;
    pwm_regs->CHN0_DATA = data;
    
    // Enable Channel 0 in Mark/Space mode
    // MSEN1 = bit 7, PWEN1 = bit 0
    pwm_regs->CONTROL = (1 << 7) | (1 << 0);
}

/**
 * @brief Sets the function mode for a specific GPIO pin.
 */
void set_gpio_mode(bcm2711_gpio_registers_t *regs, u8 pin, u8 mode) {
    u32 bank = pin / 10;
    u32 bit_pos = (pin % 10) * 3;

    // Clear the current 3 bits for this pin, then set the new mode
    regs->GPFSEL[bank] &= ~(0b111 << bit_pos);
    regs->GPFSEL[bank] |= (mode << bit_pos);
}

