#include <cc430f5137.h>


#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
        P3OUT |= 0x05;
        __delay_cycles(50000);
        P3OUT &= ~0x05;
	P2IFG &= ~BIT3; // P2.3 IFG cleared
	__bic_SR_register_on_exit(CPUOFF);
}
