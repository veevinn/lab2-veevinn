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
	if (strcmp(code,".") == 0) {
		sprintf(answer, "E");
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
			
			
			decode(code);
			code[0] = '\0';
			count = 0;
			UART_putstring(answer);
					
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
