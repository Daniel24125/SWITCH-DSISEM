/*

* Trabalho 2 ex 1.c

*

* Created: 21/03/2026 11:54:04

* Author : Daniel

Funcionalidade 1

Pretende-se controlar a velocidade de um motor DC utilizando PWM.

Os interruptores SW1, SW2, permitem incrementar (SW1) e

decrementar (SW2) em 5 unidades o valor percentual da velocidade

nominal de rotação do motor. Os interruptores SW3 e SW4 permitem

definir as velocidades de rotação do motor com os valores 25% e 50%,

da velocidade nominal, respetivamente. Sempre que for acionado um dos

interruptores o valor percentual da velocidade de rotação do motor

deverá ser apresentado no display. SW5 inverte o sentido de rotação e

SW6 deve parar o motor.
 
 
SW1 -> vel += 5

SW2 -> vel -= 5;

SW3 -> vel = 25

SW4 -> vel = 50

SW5 -> vel = -vel

SW6 -> vel = 0

Funcionalidade 2 

Utilizando a USART1 para uma comunicação série assíncrona (RS232), 9600 bps, 8 bit de dados e 2 stop bit, pretende-se fazer o controlo da velocidade do motor através do PC. Os caracteres a enviar para o ATmega 128 são os seguintes:
Carácter
Descrição
“P” ou “p” - Motor parado
“1” - 25% da velocidade nominal (sentido definido por SW5)
“2” - 50% da velocidade nominal (sentido definido por SW5)
“+” - Incrementa 5% da velocidade nominal
“-” - Decrementa 5% da velocidade nominal
“I” ou “i” - Inverter sentido de rotação
“B” ou “b” - Pedido de envio do duty cycle actual do motor (em percentagem)

Para selecionar o modo de funcionamento do sistema deve-se adicionar 2 novos caracteres. O carácter “S” seleciona o modo de funcionamento Switches (controlado pelos switches) e o carácter “D” seleciona o modo de funcionamento Digital (controlado pelo PC).
O sistema deve começar com o modo de funcionamento digital selecionado. O display 3 deve indicar sempre o modo de funcionamento ativo.


Funcionamento 3:

Pretende-se alterar o funcionamento 2 por forma a controlar a velocidade do motor através do valor da tensão aos terminais de um potenciómetro, de tal maneira que a variação de 0 V a Vcc provoca um incremento da velocidade de 0 RPM até à velocidade nominal (0 V deve corresponder a 0% da velocidade nominal e Vcc a 99% da velocidade nominal). Neste modo de funcionamento, SW5 inverte o sentido de rotação.
Deverá ser adicionado um novo modo de funcionamento (Modo Analógico “A”)

Aquisição de dados analógicos:
A conversão analógica/digital da tensão aos terminais do potenciómetro deve ser obtida a partir da média de 4 leituras consecutivas do conversor A/D (ADC0), com uma precisão de 8 bit.

*/
 
#include <avr/io.h>

#include <avr/interrupt.h>

#include <stdio.h>
 
 #define MODO_SWITCH 0 
 #define MODO_DIGITAL 1
 #define MODO_ANALOGICO 2
 
#define DISPLAY1 0b00000000
#define DISPLAY2 0b01000000
#define DISPLAY3 0b10000000
#define DISPLAY4 0b11000000
 
 
 // Definir os Segmentos
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
#define MINUS 0b10111111
#define S_CHAR 0b10010010
#define D_CHAR 0b11000000
#define A_CHAR 0b11001000
 
#define SW1 0b11111110
#define SW2 0b11111101
#define SW3 0b11111011
#define SW4 0b11110111
#define SW5 0b11101111
#define SW6 0b11011111

uint16_t timerCounter = 0;

 /*
	btnIsPressed: Flag para detetar se algum botão foi pressionado de modo a implementar o debounce e evitar que a velocidade seja incrementada demasiado rápido;
	direction: Flag que determina em que direção está a rodar o motor;
	vel: Variável inteira que guarda a atual velocidade do motor
	ocr2MaxValue: Máximo valor que o ORC2 pode ter de modo a que não ocorra overflow;
	velMax: Máxima velocidade em percentagem que o motor pode ter de modo a não ocupar 3 displays
	isDirectionInverted: Flag que determina se a direção foi invertida ou não
	communicationMode: Variável que guarda o modo de funcionamento do sistema -  “S” seleciona o modo de funcionamento Switches (controlado pelos switches) e o carácter “D” seleciona o modo de funcionamento Digital;
	buf: Buffer que irá receber os dados do modo USART;
	mode: Variável para guardar o modo de funcionamento - 0 corresponde ao modo SWITCH e 1 corresponde ao modo DIGITAL;
 */

uint8_t btnIsPressed = 0, 
direction = 1,
vel = 0,
ocr2MaxValue = 100,
velMax = 95,
isDirectionInverted = 0,
communicationMode=0,
mode = MODO_DIGITAL;


uint8_t SEG[10] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
char display[4], buf;
 
void setNumberToDisplay(void){
	sprintf(display, "%04d", vel);
	uint8_t portDState = (PORTA>>6) & 0x03;
	if (portDState == 1 && direction == 0) {
		PORTC = MINUS;
	}
	else if (display[portDState] >= '0' && display[portDState] <= '9') {
		PORTC = SEG[display[portDState] - '0'];
	}
	else if (display[portDState] == 0){
		switch(mode){
			case MODO_SWITCH:
				PORTC = S_CHAR;
				break;
			case MODO_DIGITAL:
				PORTC = D_CHAR;
				break;
			case MODO_ANALOGICO:
				PORTC = A_CHAR;
				break;
		}
		PORTC = mode == 0 ? S_CHAR : D_CHAR;
	}
	else {
		PORTC = 0xFF; 
	}
}


// Configuração dos Timers
ISR(TIMER0_COMP_vect){
	PORTA +=0x40;
	setNumberToDisplay();
	timerCounter++;
}

ISR(TIMER2_COMP_vect){
}


// Funções Init
void initMotorPins() {
	// Colocar o PB5 (Dir0), PB6 (Dir1), e PB7 (OC2) como outputs
    DDRB |= (1 << PB5) | (1 << PB6) | (1 << PB7);
    PORTB |= (1 << PB5);  
    PORTB &= ~(1 << PB6);
}
 
void initTiter1(void){
	//Set Timer0 as CTC mode, with 5ms counter
	TCCR0 |= (1<<WGM01);
	TCCR0 |= (1<<CS02) | (1<<CS01) | (1<<CS00);
	TIMSK |= (1<<OCIE0);
	TCNT0 = 0;
	OCR0 = 77;
}
 
void initTimer2(void){
	//Set Timer2 as PWM mode, with around 500Hz
	TCCR2 |= (1<<WGM20);
	TCCR2 |= (1<<COM20) | (1<<COM21);
	TCCR2 |= (1<<CS21) | (1<<CS20);
	TIMSK |= (1<<OCIE2);
	OCR2 = 0;
}
  
 void initUSART(void){
	 // Configurar a Baud Rate (9600 bps)
	 UBRR1H = 0;
	 UBRR1L = 207;
	 UCSR1A = (1<<U2X1); 
	 
	 // Ativar o Transmitter e Receiver
	 UCSR1B |= (1<<RXEN1) | (1<<TXEN1);

	 // Configurar o Frame Format (8 bits de dados, 2 stop bits, sem paridade)
	 UCSR1C |= (1 << USBS1) | (1 << UCSZ11) | (1 << UCSZ10); 
 }
 
 void ADC_Init(void){ 
	 ADMUX = (1 << REFS0) | (1 << ADLAR);
	 ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
 }
 
void init(void){
	DDRC = 0b11111111;
	DDRF = 0b00000000;
	DDRA = 0b11000000;
	
	initMotorPins();
	initTiter1();
	initTimer2();
	initUSART();
	ADC_Init();
	sei();
}
 
 
 // Funções utilitárias
void updateVel(uint8_t v){
	vel = v;
	OCR2 = ocr2MaxValue * (1 - vel)/100; // (vel * ocr2MaxValue) / 100;
}
 
uint8_t isButtonPressed(uint8_t btn){
	uint8_t pressed = 0;
	while ((PINA | btn) == btn) { pressed = 1; }
	return pressed;
}
 
void toggleDirection(void) {
	// Esperar que 250 ms passem
	while(timerCounter < 50);
	direction = !direction;
	PORTB ^= (1 << PB5);
	PORTB ^= (1 << PB6);
	isDirectionInverted = 0;
}
 
 void increaseVelocity(void){
	 updateVel(vel < velMax ? vel + 5 : vel);
 }
 
 void decreaseVelocity(void){
	 updateVel(vel > 0 ? vel - 5 : 0);
 }
 
 void invertVelocity(void){
	 isDirectionInverted = 1;
	 updateVel(0);
	 timerCounter = 0;
	 toggleDirection();
 }
 
 void USART1_Transmit(uint8_t data) {
	 // Esperar que o buffer de transmissão esteja vazio
	 while ((UCSR1A & (1 << UDRE1)) == 0);
	 // Enviar o dado
	 UDR1 = data;
 }
 
 uint8_t USART1_Receive(void) {
	 // Esperar até que um dado seja recebido (RXC1 = 1)
	 while ((UCSR1A & (1 << RXC1)) == 0);
	 // Ler e devolver o dado do buffer
	 return UDR1;
 }
 
 uint8_t ADC_Read_Avg4(void) {
	 uint16_t soma_adc = 0;
	 for (int i = 0; i < 4; i++) {
		 // Iniciar a conversão
		 ADCSRA |= (1 << ADSC);
		 // Esperar que o bit ADSC passe a 0
		 while (ADCSRA & (1 << ADSC));
		 soma_adc += ADCH;
	 }
	 // Retornar a média das 4 leituras (valor entre 0 e 255)
	 return (uint8_t)(soma_adc / 4);
 }
 
 void listenForOperationMode(void){
	 buf = USART1_Receive();
	 switch (buf) {
		case 'S':
		case 's':
			 mode = MODO_SWITCH;
			 break;
		case 'D':
		case 'd':
			 mode = MODO_DIGITAL;
			 break;
		case 'A':
		case 'a':
			mode = MODO_ANALOGICO;
			break;
	 }
 }
 
  
 // Modos de Operação
 void runSwitchMode(void){
	if(isButtonPressed(SW1)){
		 increaseVelocity();
	}else if(isButtonPressed(SW2)){
		decreaseVelocity();
	}else if(isButtonPressed(SW3)){
		updateVel(25);
	}else if(isButtonPressed(SW4)){
		updateVel(50);
	}else if(isButtonPressed(SW5)){
		invertVelocity();
	}else if(isButtonPressed(SW6)){
		updateVel(0);
	}
 }

 void runDigitalMode(void){
	switch (buf) {
		case 'P':
		case 'p':
			updateVel(0); // Motor parado
			break;
		case '1':
			updateVel(25); // 25% da velocidade nominal
			break;
		case '2':
			updateVel(50); // 50% da velocidade nominal
			break;
		case '+':
			increaseVelocity();
			break;
		case '-':
			decreaseVelocity();
			break;
		case 'I':
		case 'i':
			invertVelocity();
			break;
		case 'B':
		case 'b':
			// Pedido de envio da velocidade atual via USART1
			USART1_Transmit(vel);
		break;
	}
	 
 } 
 
 void runAnalogMode(void){
	  uint8_t media_potenciometro = ADC_Read_Avg4();
	   // Converter o valor do ADC (0 a 255) para percentagem (0 a 99%)
	  uint8_t convertedVelocity = (media_potenciometro * 99) / 255; 
	  updateVel(convertedVelocity);
 }
 
int main(void)
{
    init();
	
    while (1) 
    {
		listenForOperationMode();
		if(mode == MODO_SWITCH){
			runSwitchMode();
		}else if(mode == MODO_DIGITAL){
			runDigitalMode();
		} else if(mode == MODO_ANALOGICO){
			runAnalogMode();
		}
    }
}
 
 