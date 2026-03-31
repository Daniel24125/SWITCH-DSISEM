/*
 * ex2.c
 *
 * Created: 07/03/2026 11:43:03
 * Author : danie
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED1 0b11111110
#define LED2 0b11111101
#define LED3 0b11111011
#define LED4 0b11110111
#define LED5 0b11101111
#define LED6 0b11011111
#define LED7 0b10111111 
#define LED8 0b01111111

#define SW1 0b11111110
#define SW2 0b11111101
#define SW6 0b11011111

int main(void)
{

	/* Replace with your application code */
	volatile uint16_t counter;
	volatile uint16_t sample;
	volatile uint8_t led_activation_sequence_time = 100;
	volatile uint8_t led_deactivation_sequence_time = 50;
	
	DDRA = 0b11000000;
	PORTA = 0b11000000;
	DDRC = 0b11111111;
	PORTC = 0b11111111;
	
	//Set timer to CTC mode
	TCCR0 |= (1<<WGM01);
	TCCR0 |= (1<<COM00);
	TCCR0 |= (1 << CS01) | (1 << CS00) | (1 << CS02);
	
	TIMSK |= (1 << TOIE0);
	
	//OCR0 for 10ms
	OCR0 = 0x9C;
	
	
	//Define the prescaler
	
	
	
	while (1)
	{
		
		
	}
}




