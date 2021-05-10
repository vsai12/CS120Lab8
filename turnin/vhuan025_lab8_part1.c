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


enum O_STATES { O_Start, O_Wait, O_Press1, O_Press2, O_Press3 } O_State;
void OSwitch() {
	unsigned char tmpA = ~PINA & 0x07;
	switch(O_State) {
		case O_Start:
			O_State = O_Wait;
			break;
		case O_Wait:
			if(tmpA == 0x01)
				O_State = O_Press1;
			else if(tmpA == 0x02)
				O_State = O_Press2;
			else if(tmpA == 0x04)
				O_State = O_Press3;
			else
				O_State = O_Wait;
			break;
		case O_Press1:
			if(tmpA == 0x01)
				O_State = O_Press1;
			else
				O_State = O_Wait;
			break;
		case O_Press2:
                        if(tmpA == 0x02)
                                O_State = O_Press2;
                        else
                                O_State = O_Wait;
                        break;
		case O_Press3:
                        if(tmpA == 0x04)
                                O_State = O_Press3;
                        else
                                O_State = O_Wait;
                        break;
	}
	switch(O_State) {
		case O_Start:
			break;
		case O_Wait:
			set_PWM(0);
			break;
		case O_Press1:
			set_PWM(261.63);
			break;
		case O_Press2:
                        set_PWM(293.66);
                        break;
		case O_Press3:
                        set_PWM(329.63);
                        break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	O_State = O_Start;
	PWM_on();
    /* Insert your solution below */
    while (1) {
	    OSwitch();
    }
    return 1;
}
