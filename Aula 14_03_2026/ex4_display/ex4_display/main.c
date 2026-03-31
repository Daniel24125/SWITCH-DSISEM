/*
 * ex4_display.c
 *
 * Created: 14/03/2026 10:37:35
 * Author : Daniel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <time.h>

#define DISPLAY1 0b00000000
#define DISPLAY2 0b01000000
#define DISPLAY3 0b10000000
#define DISPLAY4 0b11000000

#define NUM0 0b11000000
#define NUM1 0b11111001
#define NUM2 0b10100100
#define NUM3 0b10110000
#define NUM4 0b10011001
#define NUM5 0b10010010
#define NUM6 0b10000010
#define NUM7 0b11111000
#define NUM8 0b10000000
#define NUM9 0b10010000



uint16_t timerCounter = 0;

//0 - no action; 1 - 2ms hit; 2 - 10ms hit; 3 - 600ms hit
uint8_t state = 0;

uint8_t D1_NUMS[10] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
uint8_t D2_NUMS[10] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
uint8_t D3_NUMS[10] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
uint8_t D4_NUMS[10] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
	
uint8_t CURRENT_DISPLAY[4] = {DISPLAY1, DISPLAY2, DISPLAY3, DISPLAY4};
uint8_t *CURRENT_NUMS[4] = {D1_NUMS, D2_NUMS, D3_NUMS, D4_NUMS};

uint8_t CURRENT_INDEX[4] = {0, 0, 0, 0};

void resetIndex(void){
	CURRENT_INDEX[0] = 0;
	CURRENT_INDEX[1] = 0;
	CURRENT_INDEX[2]= 0;
	CURRENT_INDEX[3]= 0;
	
}

void incrementDisplayIndex(uint8_t display){	
	CURRENT_INDEX[display] = (CURRENT_INDEX[display] == 8) ? 0 : CURRENT_INDEX[display] +1;
}

ISR(TIMER0_COMP_vect){
	if(timerCounter % 5 == 0){
		incrementDisplayIndex(0);
	}else if(timerCounter % 50 == 0){
		incrementDisplayIndex(1);
	}else if(timerCounter % 500 == 0){
		incrementDisplayIndex(2);
	}else if(timerCounter % 5000 == 0){
		incrementDisplayIndex(3);
	}

	
	PORTD = CURRENT_DISPLAY[timerCounter % 4];
	PORTC = CURRENT_NUMS[timerCounter % 4][CURRENT_INDEX[timerCounter % 4]];

	
	if(timerCounter % 2 == 0){
		state = 1;
		
	//10ms
	}else if(timerCounter % 5 == 0){
		state = 2;
	} else if(timerCounter % 600 == 0){
		state = 3;
		
		//resetIndex();
	}
	
	timerCounter++;

	//PORTD+=0x40;
}



int main(void)
{
	//Registo da porta C - Display - Tudo entrada
    DDRC = 0b11111111;
	DDRD = 0b11000000;
	
	TIMSK |= (1<<OCIE0);
	OCR0 = 31;
	
	//Definir o Prescaler - Neste caso para 1024
	TCCR0 |= (1<<CS00) | (1<<CS02);
	
	// Define o modo to Timer
	TCCR0 |= (1<<WGM01);
	
	//Enable Interrupt - sem isto as interrupções nunca chegam a ser emitidos
	sei();
	
    while (1) 
    {
		
    }
}

