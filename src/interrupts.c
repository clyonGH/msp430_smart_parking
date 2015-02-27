/* Code written by Celine LY (2015) */

#include <msp430.h>
#include <stdint.h>
#include "RF1A.h"
#include "rf_communication.h"
#include "led_commands.h"

#define BUFFER_SIZE 32

unsigned char buffer[BUFFER_SIZE];
unsigned char ptr_buf = 1; // First byte of the buffer reserved for the size
unsigned char id_src = 0;
unsigned char id_dest = 0;
unsigned char state_sensor = IDLE;
unsigned char state_hub = RCV_STATE;
extern unsigned char waiting_for_ack;


static void RF1A_interface_error_handler(void){
	switch(RF1AIFERRV){
		case 0: break; // No error
		case 2: // Low core voltage error
			__no_operation();
			break;
		case 4: // Operand Error
			__no_operation();
			break;
		case 6: // Output data not available error
			__no_operation();
			break;
		case 8: // Operand overwrite error
			__no_operation();
			break;
	}
}

// If RF1A_interrupt_handler is called, an interface interrupt has occured.
static void RF1A_interrupt_handler(void){
// RF1A interrupt is pending
	switch(RF1AIFIV){
		case 0: break; // No interrupt pending
		case 2: // RFERRIFG
			RF1A_interface_error_handler();
			break;
		case 4: break; // RFDOUTIFG
		case 6: break; // RFSTATIFG
		case 8: break; // RFDINIFG
		case 10: break; // RFINSTRIFG
		case 12: break; // RFRXIFG
		case 14: break; // RFTXIFG
	}
}

// RF interrupt
#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void){
	switch(RF1AIV){ // Prioritizing Radio Core Interrupts
		case 0:
			RF1A_interrupt_handler(); // RF1A interface interrupt pending
			break;
		case 2: break; // RFIFG0
		case 4: break; // RFIFG1 - GD01 = LNA_PD signal
		case 6: break; // RFIFG2
		case 8: break; // RFIFG3
		case 10: // RFIFG4 - RX end-of-packet
			packet_received();
			break;
		case 12: break; // RFIFG5
		case 14: break; // RFIFG6
		case 16: break; // RFIFG7
		case 18: break; // RFIFG8
		case 20: // RFIFG9 - TX end-of-packet
			Strobe(RF_SFTX);
			if(TX)
				ReceiveOn();
			if(RX){
				if(state_hub == SEND_ACK){
					// Ack for sensors sent
					stop_led();
					__delay_cycles(10000);
					if(id_src == 12)
						set_color(0x80, 0x80, 0x00, 0x00, 0x00, 0x00);
					else if(id_src == 43)
						set_color(0x00, 0x00, 0x00, 0x80, 0x80, 0x00);	
					__delay_cycles(10000);
					// Next step: forwarding the state
					state_hub = FW_STATE;
					if(NEXT_DEVICE != 0){
    	                forward_state(id_src, state_sensor);
						set_color(0x00, 0x80, 0x00, 0x00, 0x80, 0x00);
						waiting_for_ack = 1;
					}else{
						ReceiveOn();
						// Setting state to forward to the serial port
        	            state_hub = RCV_STATE + FW_SERIAL;
					}
				}else{
					// State forwarded to next hub
					ReceiveOn();
					__delay_cycles(10000);
				}
			}
			stop_led();
			__delay_cycles(10000);
			break;	
		case 22: break; // RFIFG10
		case 24: break; // RFIFG11
		case 26: break; // RFIFG12
		case 28: break; // RFIFG13
		case 30: break; // RFIFG14 - WOR_EVENT0
		case 32: break; // RFIFG15 - WOR_EVENT1
	}
}

// Switch interrupt
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
	if(TX){
		waiting_for_ack = 1;
		send_state();
		__delay_cycles(10000);

		P2IFG &= ~BIT3; // P2.3 IFG cleared
		P2IES ^= BIT3; // toggling transition (htl ot lth)
		P2IE |= BIT3; // P2.3 interrupt enabled
	}
}

// Serial interrupt
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
	if(RX){
		switch(UCA0IV){
    		case 0: break;                           // Vector 0 - no interrupt
			case 2:                                  // Vector 2 - RXIFG
//				while (!(UCA0IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
//				UCA0TXBUF = UCA0RXBUF;            // TX -> RXed character
				break;
			case 4:									 // Vector 4 - TXIFG
				break;
			default: break;
		}
		P2IFG &= ~BIT3; // P2.3 IFG cleared
		P2IFG &= ~BIT7; // P2.7 IFG cleared
		P2IE |= BIT3 + BIT7; // P2.3 and P2.7 interrupts enabled
	}
}
