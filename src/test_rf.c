#include <cc430f5137.h>
#include <intrinsics.h>
#include "../include/colours.h"

void init_LED(unsigned char);
void init_inputP2(unsigned char);
void blinking_LED(unsigned char);
void test_switch(unsigned char, unsigned char);

void init_LED(unsigned char colour){
	P3DIR |= colour;
}

void init_inputP2(unsigned char inputBit){
	P2IE |= inputBit; // enable interrupt
        P2IES |= inputBit; // high-to-low
        P2DIR &= inputBit;
	P2IFG &= ~inputBit;
}

void blinking_LED(unsigned char colour){
	__delay_cycles(50000);
        P3OUT |= colour;
        __delay_cycles(50000);
        P3OUT &= ~colour;
}

void test_switch(unsigned char inputBit, unsigned char colour){
	if((P2IN & inputBit) == inputBit)
        	P3OUT |= colour;
        else
        	P3OUT &= ~colour;	
}

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	init_LED(LED1_RB);
	init_inputP2(BIT3);
	
	__bis_SR_register(CPUOFF + GIE); // global interrupts enabled and low power mode enabled

	while(1){}
}
