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
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long timer = 1;
unsigned long timer_count = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	timer_count = timer;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	timer_count--;
	if(timer_count == 0) {
		TimerISR();
		timer_count = timer;
	}
}

void TimerSet(unsigned long M) {
	timer = M;
	timer_count = timer;
}



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

unsigned char playing = 0;
void playMelody() {
        double freq[8] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};
        unsigned int numNotes = 7;
        unsigned int melody[7] =  {0,1,2,0,2,0,2};
        unsigned int timeHeld[7] = {1,1,1,1,1,1,1};
        unsigned int timeBetween[6] = {1,1,1,1,1,1};
	//unsigned int timeBetween[6] = {1,0,1,0,1,0};
        static unsigned int count = 0;
        static unsigned int note = 0;
        static unsigned char playWait = 0;
        if(!playWait) {
                if(count < timeHeld[note]) {
                        set_PWM(freq[melody[note]]);
                        ++count;
                }
                else {
                        count = 0;
                        if(note < numNotes - 1) {
                                if(timeBetween[note] != 0) {
                                        playWait = 1;
                                }
                                else {
                                        ++note;
                                        set_PWM(freq[melody[note]]);
                                        ++count;
                                }
                        }
                        else {
                                playing = 0;
                                note = 0;
                        }
                }
        }
        if(playWait) {
                set_PWM(0);
                ++count;
                if(count == timeBetween[note]) {
                        playWait = 0;
                        ++note;
			count = 0;
                }
        }
	if(note == 5) {
		PORTB |= 0x01;
	}
	else
		PORTB &= 0xFE;
}

enum O_STATES { O_Start, O_Wait, O_On } O_State;
void OSwitch() {
	unsigned char tmpA = ~PINA & 0x01;
	switch(O_State) {
		case O_Start:
			O_State = O_Wait;
			break;
		case O_Wait:
			if(tmpA) {
				O_State = O_On;
				playing = 1;
			}
			else
				O_State = O_Wait;
			break;
		case O_On:
			if(!tmpA && !playing)
				O_State = O_Wait;
			else
				O_State = O_On;
			break;
	}
	switch(O_State) {
		case O_Start:
			break;
		case O_Wait:
			break;
		case O_On:
			playMelody();
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(300);
	TimerOn();
	O_State = O_Start;
	PWM_on();
    /* Insert your solution below */
    while (1) {
	    //OSwitch();
	    playMelody();
	    while(!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}
