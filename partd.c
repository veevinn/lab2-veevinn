/*
 * GccApplication3.c
 *
 * Created: 2/8/2022 5:42:53 PM
 * Author : veevinn
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)


/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

volatile long rising_edge = 0;
volatile long falling_edge = 0;
volatile long period = 0;
volatile int overflow = 0;
volatile int falling = 1;
int count = 0;

char code[6];
char answer[6];
char String[25];
char debug[100];

void Initialize() {

	
	cli();
	
	TCCR1B = 0;
	TCCR1A = 0;

	DDRB |= (1<<DDB5);
	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);
	
	DDRB &= ~(1<<DDB0);
	DDRD &= ~(1<<DDD7);
	
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12);
	
	TCCR1B |= (1<<ICES1);
	
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	
	TIFR1 |= (1<<ICF1);
	TIFR1 |= (1<<TOV1);
	
	TIMSK1 |= (1<<ICIE1);
	TIMSK1 |= (1<<TOIE1);
	
	sei();
}

void decode(char* code) {
	if (strcmp(code,".-") == 0) {
		sprintf(answer, "A");
	}
	else if (strcmp(code,"-...") == 0) {
		sprintf(answer, "B");
	}
	else if (strcmp(code,"-.-.") == 0) {
		sprintf(answer, "C");
	}
	else if (strcmp(code,"-..") == 0) {
		sprintf(answer, "D");
	}
	else if (strcmp(code,".") == 0) {
		sprintf(answer, "E");
	}
	else if (strcmp(code,"..-.") == 0) {
		sprintf(answer, "F");
	}
	else if (strcmp(code,"--.") == 0) {
		sprintf(answer, "G");
	}
	else if (strcmp(code,"....") == 0) {
		sprintf(answer, "H");
	}
	else if (strcmp(code,"..") == 0) {
		sprintf(answer, "I");
	}
	else if (strcmp(code,".---") == 0) {
		sprintf(answer, "J");
	}
	else if (strcmp(code,"-.-") == 0) {
		sprintf(answer, "K");
	}
	else if (strcmp(code,".-..") == 0) {
		sprintf(answer, "L");
	}
	else if (strcmp(code,"--") == 0) {
		sprintf(answer, "M");
	}
	else if (strcmp(code,"-.") == 0) {
		sprintf(answer, "N");
	}
	else if (strcmp(code,"---") == 0) {
		sprintf(answer, "O");
	}
	else if (strcmp(code,".--.") == 0) {
		sprintf(answer, "P");
	}
	else if (strcmp(code,"--.-") == 0) {
		sprintf(answer, "Q");
	}
	else if (strcmp(code,".-.") == 0) {
		sprintf(answer, "R");
	}
	else if (strcmp(code,"...") == 0) {
		sprintf(answer, "S");
	}
	else if (strcmp(code,"-") == 0) {
		sprintf(answer, "T");
	}
	else if (strcmp(code,"..-") == 0) {
		sprintf(answer, "U");
	}
	else if (strcmp(code,"...-") == 0) {
		sprintf(answer, "V");
	}
	else if (strcmp(code,".--") == 0) {
		sprintf(answer, "W");
	}
	else if (strcmp(code,"-..-") == 0) {
		sprintf(answer, "X");
	}
	else if (strcmp(code,"-.--") == 0) {
		sprintf(answer, "Y");
	}
	else if (strcmp(code,"--..") == 0) {
		sprintf(answer, "Z");
	}
	else if (strcmp(code,".----") == 0) {
		sprintf(answer, "1");
	}
	else if (strcmp(code,"..---") == 0) {
		sprintf(answer, "2");
	}
	else if (strcmp(code,"...--") == 0) {
		sprintf(answer, "3");
	}
	else if (strcmp(code,"....-") == 0) {
		sprintf(answer, "4");
	}
	else if (strcmp(code,".....") == 0) {
		sprintf(answer, "5");
	}
	else if (strcmp(code,"-....") == 0) {
		sprintf(answer, "6");
	}
	else if (strcmp(code,"--...") == 0) {
		sprintf(answer, "7");
	}
	else if (strcmp(code,"---..") == 0) {
		sprintf(answer, "8");
	}
	else if (strcmp(code,"----.") == 0) {
		sprintf(answer, "9");
	}
	else if (strcmp(code,"----") == 0) {
		sprintf(answer, "0");
	}
}

void reset(char* input) {
	for (int i = 0; i < 6; i++) {
		input[i] = '\0';
	}
	
}

ISR(TIMER1_CAPT_vect){

	TIFR1 |= (1<<ICF1);
	
	if (falling) {
		
		rising_edge = ICR1;
		falling = 0;
		
		if ((overflow* 65536 + rising_edge - falling_edge) >= 25000) {
			sprintf(String, " ");
			UART_putstring(String);
			
			//UART_putstring(code);
			
			decode(code);
			reset(code);
			count = 0;
			UART_putstring(answer);
			reset(answer);
					
		}
		PORTB &= ~(1<<PORTB2);
		PORTB &= ~(1<<PORTB1);
 
	}
	else {
		falling_edge = ICR1;			
		period = (overflow * 65536) + falling_edge - rising_edge;	
		falling = 1;	
		
		if ((period >= 1875) && (period < 12500)) {
			PORTB |= (1<<PORTB1);
			PORTB &= ~(1<<PORTB2);
	
			//sprintf(String, ".");
			//UART_putstring(String);
			code[count] = '.';
			count++;
			
		}
		else if (period >= 12500) {
			PORTB |= (1<<PORTB2);
			PORTB &= ~(1<<PORTB1);
	
			//sprintf(String, "-");
			//UART_putstring(String);
			code[count] = '-';
			count++;
		}

		else {
			PORTB &= ~(1<<PORTB2);
			PORTB &= ~(1<<PORTB1);
		}
	}
	
	
	overflow = 0;
	
	
	
	
	TCCR1B ^= (1<<ICES1);
}

ISR(TIMER1_OVF_vect){
	overflow++;
}

int main(void)
{
	Initialize();
	UART_init(BAUD_PRESCALER);

	while(1);
}

