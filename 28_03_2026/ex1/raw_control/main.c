#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define ledPin 12
#define RPI4_PERIPH_BASE 0xFE000000
#define RPI4_GPIO_BASE (RPI4_PERIPH_BASE + 0x200000)
#define ADDRESS_RANGE (4 * 1024) // 4KB memory page size

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

int main() {
    int mem_fd;
    void *gpio_map;
    
    // volatile ensures the compiler writes directly to RAM/Hardware every time
    bcm2837_gpio_registers_t *gpio_regs;

    // 1. Open the raw memory file
    mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (mem_fd < 0) {
        printf("Error: Cannot open /dev/mem. Are you running as root (sudo)?\n");
        return -1;
    }

    // 2. Map the physical GPIO addresses to our virtual memory space
    gpio_map = mmap(
        NULL,
        ADDRESS_RANGE,
        PROT_READ|PROT_WRITE, 
        MAP_SHARED,
        mem_fd,
        RPI4_GPIO_BASE 
    );

    close(mem_fd); // We can safely close the file descriptor after mapping

    if (gpio_map == MAP_FAILED) {
        printf("Error: mmap failed!\n");
        return -1;
    }

    // Point our struct to the newly mapped memory block
    gpio_regs = (bcm2837_gpio_registers_t *)gpio_map;

    // 3. Configure the pin as an output
    uint32_t rpi_bank = ledPin / 10;
    uint32_t rpi_pin_pos = (ledPin % 10) * 3;

    // Clear the bits, then set the GPIO as output
    gpio_regs->GPFSEL[rpi_bank] &= ~(7 << rpi_pin_pos);
    gpio_regs->GPFSEL[rpi_bank] |= (1 << rpi_pin_pos);

    // 4. Turn the LED ON
    uint32_t gpio_bank = ledPin / 32;
    uint32_t gpio_pin_pos = (ledPin % 32);

    // Note: For GPSET and GPCLR, writing a 1 executes the action. Writing a 0 does nothing.
    // Therefore, simple assignment (=) is preferred over bitwise OR (|=) for these specific registers.
    gpio_regs->GPSET[gpio_bank] = (1 << gpio_pin_pos);

    printf("LED 12 should now be ON!\n");

    return 0;
}