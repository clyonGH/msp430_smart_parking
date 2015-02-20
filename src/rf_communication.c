#include <msp430.h>
#include <stdint.h>
#include "RF1A.h"
#include "RF_Connection.h"
#include "led_commands.h"
#include "rf_communication.h"

#define BUFFER_SIZE 32 // Warning: the buffer size necessarily includes 2 bytes: RSSI and LQI

extern unsigned char buffer[BUFFER_SIZE];
extern unsigned char ptr_buf;
extern unsigned char waiting_for_ack;
extern unsigned char id_src;
extern unsigned char id_dest;
extern unsigned char state_sensor;
extern unsigned char waiting_for_ack;
extern unsigned char state_hub;

uint8_t RxBufferLength;
uint8_t RxBuffer[BUFFER_SIZE];

void send_ack(unsigned char source){
	// Current step: sending the ack
	state_hub = SEND_ACK;
	__delay_cycles(100000);

	unsigned char buffer_ack[BUFFER_SIZE];
	buffer_ack[0] = 1;
	buffer_ack[1] = source;
	rf_transmit(buffer_ack, 2);
	__delay_cycles(100000); // Delay to allow a proper transmission
}

void packet_received(void){
	RxBufferLength = ReadSingleReg(RXBYTES);
	ReadBurstReg(RF_RXFIFORD, RxBuffer, RxBufferLength);

	if(RX){
		if((state_hub & RCV_STATE) == RCV_STATE){
			id_src = RxBuffer[1];
			id_dest = RxBuffer[2];
			// If right destination
			if(id_dest == ID_DEVICE){
				switch(RxBuffer[3]){
					case 'g':
						state_sensor = 'g';
						if(id_src == 12)
							set_color(0x00, 0x00, 0x80, 0x00, 0x00, 0x00);
						else if(id_src == 43)
							set_color(0x00, 0x00, 0x00, 0x00, 0x00, 0x80);
						break;
					case 'r':
						state_sensor = 'r';
						if(id_src == 12)
							set_color(0x80, 0x00, 0x00, 0x00, 0x00, 0x00);
						else if(id_src == 43)
							set_color(0x00, 0x00, 0x00, 0x80, 0x00, 0x00);
						break;
					// Here if neither 'g' or 'r' is received, it means that a state has been forwarded, so the serial tx should start
					default:
						if(id_src == 25)
							set_color(0x80, 0x00, 0x00, 0x80, 0x00, 0x00);
						if(id_dest == 14 && id_src == 25)
							set_color(0x80, 0x80, 0x00, 0x80, 0x80, 0x00);
						if(id_dest == 14 && id_src == 25 && RxBuffer[3] == 12)
                            set_color(0x80, 0x80, 0x80, 0x80, 0x80, 0x80);						
						break;
				}
				ReceiveOff();
				// Send ack
				Strobe(RF_SFTX);
				unsigned char iAck = 0;
				for(iAck = 0; iAck < 10; iAck++)
					send_ack(id_src);
			}
		}else{
			// Checking if ack received
			if(RxBuffer[1] == ID_DEVICE){
				// Next step: receiving a new state
				ReceiveOn();
				state_hub = RCV_STATE;
				set_color(0x00, 0x80, 0x80, 0x00, 0x80, 0x80);
				waiting_for_ack = 0;
		    }
		}
	}else if(TX){
		// Checking if ack received
		if(RxBuffer[1] == ID_SRC){
			ReceiveOff();
			waiting_for_ack = 0;
		}
	}
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

// From sensor to hub
void send_state(void){
    ptr_buf = 1;
	// Data sent: size / src / dest / state_sensor
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

// From hub to hub
void forward_state(unsigned char id_sensor, unsigned char state_sensor){
	unsigned char buffer_fw[BUFFER_SIZE];
	unsigned char ptr_fw = 5;
	
	if(state_sensor == 'g')
		state_sensor = 0x01;
	else if(state_sensor == 'r')
		state_sensor = 0x00;

	// Data sent: size / src / dest / id_sensor / state_sensor
	buffer_fw[0] = 4;
	buffer_fw[1] = ID_DEVICE;
	buffer_fw[2] = NEXT_DEVICE;
	buffer_fw[3] = id_sensor;
	buffer_fw[4] = state_sensor;
	rf_transmit(buffer_fw, ptr_fw);
	__delay_cycles(10000);
	// Next step: receiving the ack
	state_hub = RCV_ACK;
}
