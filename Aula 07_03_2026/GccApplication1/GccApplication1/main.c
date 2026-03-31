/*
 * GccApplication1.c
 *
 * Created: 07/03/2026 10:36:41
 * Author : danie
 */ 

#include <avr/io.h>


int main(void)
{

    /* Replace with your application code */
	
	DDRA = 0b11000000;
	PORTA = 0b11000000;
	DDRC = 0b11111111;
	PORTC = 0b11111111;
	
	uint8_t sw1 = 0b11111110;
	uint8_t sw2 = 0b11111101;
	uint8_t sw3 = 0b11111011;
	uint8_t sw4 = 0b11110111;
	uint8_t sw6 = 0b11011111;
	
	uint8_t posrtc_state_1 = 0b01111110;
	uint8_t posrtc_state_2 = 0b10111101;
	uint8_t posrtc_state_3 = 0b11011011;
	uint8_t posrtc_state_4 = 0b11100111;
	uint8_t posrtc_state_5 = 0b11111111;

	
    while (1) 
    {
		if(PINA == sw1){
			PORTC &= posrtc_state_1;
		}else if(PINA == sw2){
			PORTC &= posrtc_state_2;
		}else if(PINA == sw3){
			PORTC &= posrtc_state_3;
		}else if(PINA == sw4){
			PORTC &= posrtc_state_4;
		}else if(PINA == sw6){
			PORTC = posrtc_state_5;
		}
    }
}

