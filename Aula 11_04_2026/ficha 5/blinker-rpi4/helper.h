#ifndef HELPER_H
#define HELPER_H

#include <linux/types.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/io.h>

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
    u32 CONTROL;
    u32 STATUS;
    u32 DMAC[2];
    u32 CHN0_RANGE; 
    u32 CHN0_DATA;  
    u32 FIF1[2];    
    u32 CHN1_RANGE;
    u32 CHN1_DATA;
} bcm2711_pwm_registers_t;

// GPIO Register Structure
typedef struct {
    u32 GPFSEL[6];
    u32 Reserved0;
    u32 GPSET[2];
    u32 Reserved1;
    u32 GPCLR[2];
    u32 Reserved2;
    u32 GPLEV[2];
} bcm2711_gpio_registers_t;

// Function Prototypes
void set_gpio_function(volatile bcm2711_gpio_registers_t *gpio_regs, u8 pin, u8 func);
int setup_pwm_clock(volatile u32 *clk_regs, u32 divider);
void setup_pwm_channel0(volatile bcm2711_pwm_registers_t *pwm_regs, u32 range, u32 data);
void set_gpio_mode(bcm2711_gpio_registers_t *regs, u8 pin, u8 mode);

#endif // HELPER_H
