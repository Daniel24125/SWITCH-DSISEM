#ifndef SETPWM_H_
#define SETPWM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

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
 * Configure the PWM in the modes needed 
 */
void set_pwm_mode(bmc2711_pwm_registers_t *regs, int rangeScale){
    regs->CHN0_RANGE = rangeScale;
}

#endif

/**
 * Escolher:
 *  Frequencia - queremos 54Mhz
 *  divisor - queremos 27
 * 
 * 
 *  N - chanel0_range
 *  duty_cycle - definido com o channel_data (DT = data/range)
 *  Ativar pwm - PWEN1
 */