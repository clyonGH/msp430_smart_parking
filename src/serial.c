#include <cc430f5137.h>
#define UART_9600

void init_serial(void){
	// Port Mapping
	PMAPPWD = 0x02D52;			// Get write-access to port mapping regs
	P2MAP3 = PM_UCA0RXD;		// Map UCA0RXD output to P2.3
	P2MAP7 = PM_UCA0TXD;		// Map UCA0TXD output to P2.7
	PMAPPWD = 0;				// Lock port mapping registers

	P2DIR |= BIT7;				// Set P2.7 as TX output
//	P2DIR &= BIT3;
	P2SEL |= BIT3 + BIT7;		// P2.3 & P2.7 to UART function

	// UART configuration
	UCA0CTL1 |= UCSWRST;		// Put state machine in reset	
	
#ifdef UART_115200
	UCA0CTL1 |= UCSSEL_2;	// SMCLK
	UCA0BR0 = 9;			// 1MHz 115200 (see User's Guide)
	UCA0BR1 = 0;			// 1MHz 115200
	UCA0MCTL = UCBRS_1 + UCBRF_0;		// Modulation UCBRSx=1, UCBRFx=0
#elif defined(UART_9600)
	UCA0CTL1 |= UCSSEL_1;	// CLK = ACLK
	UCA0BR0 = 0x03;			// 32k/9600 = 3.41
	UCA0BR1 = 0x00;
	UCA0MCTL = UCBRS_3 + UCBRF_0;	// Modulation UCBRSx=3, UCBRFx=0
#endif

	UCA0CTL1 &= ~UCSWRST;	// Initialise USCI state machine

	// Enable interruption
	UCA0IE |= UCRXIE;
}
