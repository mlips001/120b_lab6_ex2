/*
 * Lab6_ex2.c
 *
 * Created: 10/22/2019 1:21:05 PM
 * Author : Matthew L
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned char button0;
unsigned char tempB;

enum States{init, light1, wait1, light2, wait2, light3, wait3}state;

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 70;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void lightTick(){
	button0 = ~PINA & 0x01;
	
	switch (state) {
		case init:
			state = light1;
			break;
			
		case light1:
			if(button0){
				state = wait1;
			}
			else{
				state = light2;
			}
			break;
			
		case wait1:
			if(button0){
				state = light1;
			}
			else{
				state = wait1;
			}
			break;
			
		case light2:
			if(button0){
				state = wait2;
			}
			else{
				state = light3;
			}
			break;
			
		case wait2:
			if(button0){
				state = light2;
			}
			else{
				state = wait2;
			}
			break;
			
		case light3:
			if(button0){
				state = wait3;
			}
			else{
				state = light1;
			}
			break;
			
		case wait3:
			if(button0){
				state = light3;
			}
			else{
				state = wait3;
			}
			break;
	}
	
	switch (state) {
		case init:
			tempB = 0x01;
			break;
			
		case light1:
			tempB = 0x01;
			break;
			
		case wait1:
			tempB = 0x01;
			break;
			
		case light2:
			tempB = 0x02;
			break;
			
		case wait2:
			tempB = 0x02;
			break;
			
		case light3:
			tempB = 0x04;
			break;
			
		case wait3:
			tempB = 0x04;
			break;
	}
}

int main(void) {
	
	 DDRB = 0xFF; PORTB = 0x00; 
	 DDRA = 0x00; PORTA = 0xFF; 
	 TimerSet(60);
	 TimerOn();
	 
	 //state = init;
	 
    /* Replace with your application code */
    while (1) 
    {
		 //tempA = ~PINA & 0x08;
		 
		 lightTick();
		 
		 while (!TimerFlag);	
		 TimerFlag = 0;
		 
		 PORTB = tempB;
		
    }
}

