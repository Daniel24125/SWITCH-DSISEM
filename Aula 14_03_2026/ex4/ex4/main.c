/*
 * ex4.c
 *
 * Created: 14/03/2026 09:18:11
 * Author : Daniel
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

#define SW1_MASK 0b00000001
#define SW2_MASK 0b00000010
#define SW6_MASK 0b00100000

#define SECOND 100
#define HALF_SECOND 50
#define PRESSED 0b00000000


uint8_t timerCounter = 0;
uint8_t activeSeq = 0;

ISR(TIMER0_COMP_vect){
	timerCounter++;
}

void SW1_SEQ(){
	if(timerCounter == SECOND){
		PORTC = 0b11111111;
		PORTC = (PORTC<<1);
		timerCounter = 0;
	}
}
void SW2_SEQ(){
	if(timerCounter == SECOND){
		PORTC = 0b00000000;
		PORTC |= (1>>1);
		timerCounter = 0;
	}
}

int main(void)
{
	DDRA = 0b11000000;
	PORTA = 0b11000000;
	PORTC = 0b11111111;
	DDRC = 0b11111111;
	
	//Definir o modo para CTC
	TCCR0 |= (1<<WGM01);
	//Definir os prescaler
	TCCR0 |= (1<<CS00) | (1<<CS01) | (1<<CS02);
	//Definir para modo compare
	TIMSK |= (1<<OCIE0);
	
	OCR0 = 157;
	sei();
	/*
		0 - nenhum foi carregado
		1 - SW1 carregado
		2 - SW2 carregado
		6 - SW6 carregado
	*/
	uint8_t buttonPressed = 0;
	
    while (1) 
    {	
		uint8_t SW1_PRESSED = (PINA & SW1_MASK) == PRESSED;
		uint8_t SW2_PRESSED = (PINA & SW2_MASK) == PRESSED;
		uint8_t SW6_PRESSED = (PINA & SW6_MASK) == PRESSED;
		
		
	
		if(SW1_PRESSED){
			buttonPressed = 1;
			
		}
		else if(SW2_PRESSED){
			buttonPressed = 2;
		}else if(SW6_PRESSED){
			buttonPressed = 6;
		}
		
		if(buttonPressed == 1){
			activeSeq = 1;
			SW1_SEQ();
		}
		if(buttonPressed == 2){
			activeSeq = 2;
			SW2_SEQ();
		}
		if(buttonPressed == 6){
			
		}
		
    }
}

