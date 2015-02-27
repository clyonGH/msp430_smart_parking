/* Code written by Celine LY (2015) */

#include <cc430f5137.h>

// Mapping the LEDs using the PWM
void port_mapping(void){
	PMAPPWD = 0x02D52; // Get write-access to port mapping regs
	P3MAP0 = PM_TA1CCR1A; // Map TA1CCR1 output to P3.0
	P3MAP1 = PM_TA1CCR2A; // Map TA1CCR2 output to P3.1
	P3MAP2 = PM_TA0CCR1A; // Map TA0CCR1 output to P3.2
	P3MAP3 = PM_TA0CCR2A; // Map TA0CCR2 output to P3.3
	P3MAP4 = PM_TA0CCR3A; // Map TA0CCR3 output to P3.4
	P3MAP5 = PM_TA0CCR4A; // Map TA0CCR4 output to P3.5
	PMAPPWD = 0; // Lock port mapping registers
	P3DIR |= 0x3F; // P3.0 to P3.5 as output
	P3SEL |= 0x3F; // P3.0 and P3.3 options select
}

// Mapping P2.inputBit as input and enabling the interrupt
void init_inputP2(unsigned char inputBit){
	P2IE |= inputBit; // enable interrupt
	P2IES |= inputBit; // high-to-low
	P2DIR &= inputBit; // setting P2.inputBit as input
	P2IFG &= ~inputBit; // clearing the interrupt flag
}
