#include <msp430.h>
#include <stdint.h>
#include "RF1A.h"

#define BUFFER_SIZE 32
#define ID_SRC 43
#define ID_DEST 25
#define ID_DEVICE 25

void set_color(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void stop_led(void);
void send_ack(unsigned char);
void send_state(void);
unsigned char buffer[BUFFER_SIZE];
unsigned char ptr_buf = 1; // First byte of the buffer reserved for the size
unsigned char id_src = 0;
unsigned char id_dest = 0;
unsigned char waiting_for_ack = 0;

void packet_received(void){
	uint8_t RxBufferLength;
	uint8_t RxBuffer[BUFFER_SIZE];
	
	RxBufferLength = ReadSingleReg(RXBYTES);
	ReadBurstReg(RF_RXFIFORD, RxBuffer, RxBufferLength);

	if(RX){
		id_src = RxBuffer[1];
		id_dest = RxBuffer[2];
		// If right destination
		if(id_dest == ID_DEVICE){
			switch(RxBuffer[3]){
				case 'g':
					set_color(0x00, 0x00, 0x80, 0x00, 0x00, 0x80);
					break;
				case 'r':
					set_color(0x80, 0x00, 0x00, 0x80, 0x00, 0x00);
					break;
				default:
					set_color(0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
					break;
			}
			ReceiveOff();
			// Send ack
			Strobe(RF_SFTX);
			send_ack(id_src);
			ReceiveOn();
		}
	}else if(TX){
		// Checking if ack received
		if(RxBuffer[1] == ID_SRC){
			ReceiveOff();
			waiting_for_ack = 0;
		}
	}
}

void send_ack(unsigned char source){
	unsigned char buffer_ack[BUFFER_SIZE];
	buffer_ack[0] = 1;
	buffer_ack[1] = source;
	rf_transmit(buffer_ack, 2);
	__delay_cycles(100000); // Delay to allow a proper transmission
}

void send_packet(void){
	buffer[0] = ptr_buf-1;
	rf_transmit(buffer, ptr_buf);
	__delay_cycles(10000);

	switch(buffer[3]){
		case 'g':
			set_color(0x00, 0x00, 0x00, 0x80, 0x00, 0x00);
			break;
		case 'r':
			set_color(0x80, 0x00, 0x00, 0x00, 0x00, 0x00);
			break;
		default: break;
	}
}
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
// ISR
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
			ReceiveOn();
			if(RX){
				set_color(0x80, 0x80, 0x00, 0x80, 0x80, 0x00);
				__delay_cycles(10000);
				waiting_for_ack = 0;
			}
			break;	
		case 22: break; // RFIFG10
		case 24: break; // RFIFG11
		case 26: break; // RFIFG12
		case 28: break; // RFIFG13
		case 30: break; // RFIFG14 - WOR_EVENT0
		case 32: break; // RFIFG15 - WOR_EVENT1
	}
}

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
