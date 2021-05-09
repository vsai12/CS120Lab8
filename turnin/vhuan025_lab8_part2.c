/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	if(frequency != current_frequency) {
		if(!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		
		if(frequency < 0.954) { OCR3A = 0xFFFF; }

		else if(frequency > 31250) { OCR3A = 0x0000; }

		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0); //test if this works
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
enum O_STATES { O_Start, O_Off, O_On, O_Wait1, O_Wait2 } O_State;
void OSwitch() {
	unsigned char tmpA = ~PINA & 0x01;
	switch(O_State) {
		case O_Start:
			O_State = O_Wait2;
			break;
		case O_Off:
			if(!tmpA)
				O_State = O_Wait2;
			else
				O_State = O_Off;
			break;
		case O_On:
			if(!tmpA)
				O_State = O_Wait1;
			else
				O_State = O_On;
			break;
		case O_Wait1:
			if(tmpA)
				O_State = O_Off;
			else 
				O_State = O_Wait1;
			break;
		case O_Wait2:
			if(tmpA)
				O_State = O_On;
			else
				O_State = O_Wait2;
			break;
	}
	switch(O_State) {
		case O_Start:
			break;
		case O_Off:
			PWM_off();
			break;
		case O_On:
			PWM_on();
			break;
		case O_Wait1:
			break;
		case O_Wait2:
			break;
	}
}

unsigned int MAX = 7;
enum S_STATES { S_Start, S_Wait, S_Press1, S_Press2 } S_State;
void Speaker() {
	unsigned char tmpA = ~PINA & 0x06;
	double freq[8] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};
	static unsigned int counter = 0;
	//tmpA = 0x01;
	switch(S_State) {
		case S_Start:
			S_State = S_Wait;
			break;
		case S_Wait:
			if(tmpA == 0x02) {
				S_State = S_Press1;
				if(counter < MAX)
					++counter;
			}
			else if(tmpA == 0x04) {
				S_State = S_Press2;
				if(counter > 0)
					--counter;
			}
			else
				S_State = S_Wait;
			break;
		case S_Press1:
                        if(tmpA == 0x00)
                                S_State = S_Wait;
                        else
                                S_State = S_Press1;
                        break;
		case S_Press2:
                        if(tmpA == 0x00)
                                S_State = S_Wait;
                        else
                                S_State = S_Press2;
                        break;
	}
	set_PWM(freq[counter]);
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	O_State = O_Start;
	S_State = S_Start;
    /* Insert your solution below */
    while (1) {
	    OSwitch();
	    Speaker();
    }
    return 1;
}
