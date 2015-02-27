/* Code written by Celine LY (2015) */

#include <cc430f5137.h>
#include <stdint.h>
#include "RF1A.h"
#include "RF_Connection.h"
#include "ports.h"
#include "led_commands.h"
#include "rf_communication.h"
#include "serial.h"

#define BUFFER_SIZE 32 // Warning: the buffer size necessarily includes 2 bytes: RSSI and LQI

extern unsigned char buffer[BUFFER_SIZE];
extern unsigned char ptr_buf;
extern unsigned char state_hub;
extern unsigned char id_src;
extern unsigned char state_sensor;
extern uint8_t RxBufferLength;
extern uint8_t RxBuffer[BUFFER_SIZE];
unsigned char waiting_for_ack = 0;

int main(void){
	WDTCTL = WDTPW + WDTHOLD; // Stopping WDT
	if(RX)
		init_serial();
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

	set_color(0x80, 0x00, 0x00, 0x80, 0x00, 0x00);
	__delay_cycles(15000);
	stop_led();
	__delay_cycles(10000);

	while(1){
		if(TX){ // Sensor
			if(waiting_for_ack){
				stop_led();
				__delay_cycles(50000);
				send_state();
				__delay_cycles(50000);
				__delay_cycles(1000/ID_SRC); // Avoiding collisions by using the ID_SRC, which is different for each device
			}else{
				set_color(0x80, 0x00, 0x00, 0x80, 0x00, 0x00);
				__delay_cycles(50000);
			}
		}else if(RX){ // Hub
			// Waiting for ack
			if((state_hub == FW_STATE) && waiting_for_ack){
				stop_led();
                __delay_cycles(50000);
                forward_state(id_src, state_sensor);
                __delay_cycles(50000);
			}else if(!waiting_for_ack){
				set_color(0x00, 0x80, 0x80, 0x00, 0x80, 0x80);
				__delay_cycles(50000);
			}
				
			// Forward a sensor state to the serial
			if((state_hub & FW_SERIAL) == FW_SERIAL){
				
				unsigned char iBuffer = 0;
				while (!(UCA0IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
					UCA0TXBUF = RxBufferLength+'0';

				while (!(UCA0IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
                    UCA0TXBUF = ID_PARKING;
				
				for(iBuffer = 0; iBuffer <= RxBufferLength; iBuffer++) {
					while (!(UCA0IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
/*					if(RxBuffer[iBuffer] == 25)
						UCA0TXBUF = 's';
					if(RxBuffer[iBuffer] == 14)
                        UCA0TXBUF = 'd';					
					if(RxBuffer[iBuffer] == 12)
						UCA0TXBUF = 'c';
					if(RxBuffer[iBuffer] == 0)
                        UCA0TXBUF = '0';
					if(RxBuffer[iBuffer] == 1)
                        UCA0TXBUF = '1';
*/
					if((iBuffer == 3) || (iBuffer == 4))
						UCA0TXBUF = RxBuffer[iBuffer];
				}
				
				while (!(UCA0IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
                    UCA0TXBUF = '\0';
				state_hub = RCV_STATE;			
			}
		}
	}
	return 0;
}
