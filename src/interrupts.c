#include <msp430.h>
#include <stdint.h>
#include "RF1A.h"

#define BUFFER_SIZE 2

void set_color(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void stop_led(void);

unsigned char buffer[BUFFER_SIZE];
unsigned char ptr_buf = 1; // First byte of the buffer reserved for the size

void packet_received(void){
    uint8_t RxBufferLength;
    uint8_t RxBuffer[BUFFER_SIZE];

    RxBufferLength = ReadSingleReg(RXBYTES);
    ReadBurstReg(RF_RXFIFORD, RxBuffer, RxBufferLength);

    switch(RxBuffer[1]){
		case 'g':
           	set_color(0x00, 0x00, 0x80, 0x00, 0x80, 0x80);
            break;
        case 'r':
           	set_color(0x80, 0x00, 0x00, 0x00, 0x80, 0x80);
            break;
        default:
			set_color(0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
            break;
    }
}

void send_packet(void){
	buffer[0] = ptr_buf-1;
    rf_transmit(buffer, ptr_buf);

	switch(buffer[1]){
		case 'g':
			set_color(0x00, 0x00, 0x80, 0x80, 0x80, 0x00);
		break;
		case 'r':
			set_color(0x80, 0x00, 0x00, 0x80, 0x80, 0x00);
		break;
		default: break;
	}
}

static void RF1A_interface_error_handler(void){
	switch(RF1AIFERRV){
		case 0: break;                  // No error
    	case 2:                             // Low core voltage error     
			__no_operation();
    	break;
        case 4:                             // Operand Error
            __no_operation();
        break;
        case 6:                             // Output data not available error 
            __no_operation();
        break;
        case 8:                             // Operand overwrite error
            __no_operation();
        break;
	}
}

// If RF1A_interrupt_handler is called, an interface interrupt has occured.
static void RF1A_interrupt_handler(void){
	// RF1A interrupt is pending
	switch(RF1AIFIV){
 	   case 0: break;                         // No interrupt pending
       case 2:                                // RFERRIFG 
	       RF1A_interface_error_handler();
       case 4: break;                         // RFDOUTIFG
       case 6: break;                         // RFSTATIFG
       case 8: break;                         // RFDINIFG
       case 10: break;                         // RFINSTRIFG
       case 12: break;                         // RFRXIFG
       case 14: break;                         // RFTXIFG
	}
}

// ISR
#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void){
	switch(RF1AIV){        // Prioritizing Radio Core Interrupts 
    	case 0: 
			RF1A_interrupt_handler();	// RF1A interface interrupt pending
    	break;
    	case 2: break;                         // RFIFG0 
    	case 4: break;                         // RFIFG1 - GD01 = LNA_PD signal
    	case 6: break;                         // RFIFG2
    	case 8: break;                         // RFIFG3
    	case 10:  							   // RFIFG4 - RX end-of-packet
			packet_received();
		break;
		case 12: break;                         // RFIFG5
    	case 14: break;                         // RFIFG6          
    	case 16: break;                         // RFIFG7
    	case 18: break;                         // RFIFG8
    	case 20:								// RFIFG9 - TX end-of-packet
		break;
		case 22: break;                         // RFIFG10
    	case 24: break;                         // RFIFG11
    	case 26: break;                         // RFIFG12
    	case 28: break;                         // RFIFG13
    	case 30: break;                         // RFIFG14 - WOR_EVENT0
		case 32: break;							// RFIFG15 - WOR_EVENT1
	}
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
	if(TX){
		// If there is an interrupt, the state has necessarily changed
   		// So the LED1 goes from Red to Green or from Green to Red
    	if((P2IN & BIT3) == BIT3){
			// LED1: Green
    		set_color(0x00, 0x00, 0x80, 0x00, 0x00, 0x00);
			buffer[ptr_buf] = 'g';
			ptr_buf++;
		}else{
			// LED1: Red
			set_color(0x80, 0x00, 0x00, 0x00, 0x00, 0x00);
      	 	buffer[ptr_buf] = 'r';
			ptr_buf++;
		}
		send_packet();
		ptr_buf = 1;

		P2IFG &= ~BIT3; // P2.3 IFG cleared
	    P2IES ^= BIT3; // toggling transition (htl ot lth)
    	P2IE |= BIT3; // P2.3 interrupt enabled
	}
}
