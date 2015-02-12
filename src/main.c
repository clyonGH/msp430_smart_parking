#include <msp430.h>
#include <stdint.h>
#include "RF1A.h"
#include "RF_Connection.h"

#define PWM_PERIOD 255
#define BUFFER_SIZE 32 // Warning: the buffer size necessariy includes 2 bytes: RSSI and LQI
#define ID_SRC 43
#define ID_DEST 25
#define ID_DEVICE 25

void send_packet(void);
void set_color(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
extern unsigned char buffer[BUFFER_SIZE];
extern unsigned char ptr_buf;
extern unsigned char waiting_for_ack;

void send_state(void){
    ptr_buf = 1;
	buffer[ptr_buf++] = ID_SRC;
    buffer[ptr_buf++] = ID_DEST;
    // If there is an interrupt, the state has necessarily changed
    // So the LED1 goes from Red to Green or from Green to Red
    if((P2IN & BIT3) == BIT3){
    	// LED1: Green
        set_color(0x00, 0x00, 0x80, 0x00, 0x00, 0x00);
        buffer[ptr_buf++] = 'g';
   	}else{
       	// LED1: Red
        set_color(0x80, 0x00, 0x00, 0x00, 0x00, 0x00);
        buffer[ptr_buf++] = 'r';
    }
    send_packet();
}

void set_color(uint8_t red, uint8_t blue, uint8_t green, uint8_t red2, uint8_t blue2, uint8_t green2){
	// 1- Configure the PWM PERIOD
	// Same period for the two timers
	TA0CCR0 = PWM_PERIOD;
	TA1CCR0 = PWM_PERIOD;
	// 2- Set the ratio for the output
	// 0xFF = 100%, 0x00 = 0%
	// Red
	TA1CCR1 = (red*PWM_PERIOD)/0xFF;
	TA0CCR2 = (red2*PWM_PERIOD)/0xFF;
	// Blue
	TA0CCR1 = (blue*PWM_PERIOD)/0xFF;
	TA0CCR4 = (blue2*PWM_PERIOD)/0xFF;
	// Green
	TA1CCR2 = (green*PWM_PERIOD)/0xFF;
	TA0CCR3 = (green2*PWM_PERIOD)/0xFF;
	TA1CCTL1 = OUTMOD_7; // CCR1 reset/set
	TA1CCTL2 = OUTMOD_7; // CCR2 reset/set
	TA0CCTL1 = OUTMOD_7; // CCR1 reset/set
	TA0CCTL2 = OUTMOD_7; // CCR2 reset/set
	TA0CCTL3 = OUTMOD_7; // CCR3 reset/set
	TA0CCTL4 = OUTMOD_7; // CCR4 reset/set
	// Activate PWM
	TA0CTL = TASSEL_1 + MC_1 + TACLR; // ACLK, up mode, clear TAR
	TA1CTL = TASSEL_1 + MC_1 + TACLR; // ACLK, up mode, clear TAR
}

// Stopping the PWM
void stop_led(void){
	TA0CCR0 = 0;
	TA1CCR0 = 0;
	TA1CCTL1 = OUTMOD_0; // CCR1 reset/set
	TA1CCTL2 = OUTMOD_0; // CCR2 reset/set
	TA0CCTL1 = OUTMOD_0; // CCR1 reset/set
	TA0CCTL2 = OUTMOD_0; // CCR2 reset/set
	TA0CCTL3 = OUTMOD_0; // CCR3 reset/set
	TA0CCTL4 = OUTMOD_0; // CCR4 reset/set
}
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
void init_inputP2(unsigned char inputBit){
	P2IE |= inputBit; // enable interrupt
	P2IES |= inputBit; // high-to-low
	P2DIR &= inputBit; // setting P2.inputBit as input
	P2IFG &= ~inputBit; // clearing the interrupt flag
}

int main(void){
	WDTCTL = WDTPW + WDTHOLD; // Stopping WDT
	port_mapping(); // LED configuration
	rf_init();
	if(TX)
		ReceiveOff();
	_BIS_SR(GIE); // Enabling interrupts

	if(TX)
		__delay_cycles(200000);
	__delay_cycles(100000);
	
	if(RX)
		ReceiveOn();
	else if(TX){
		init_inputP2(BIT3);
	}

	while(1){
		if(TX){
			if(waiting_for_ack){
				stop_led();
				__delay_cycles(50000);
				send_state();
				__delay_cycles(50000);
			}else{
				set_color(0x80, 0x00, 0x00, 0x80, 0x00, 0x00);
				__delay_cycles(50000);
			}
		}
	}
	return 0;
}
