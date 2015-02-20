#define RCV_STATE	0x01
#define SEND_ACK	0x02
#define FW_STATE	0x03
#define RCV_ACK		0x04
#define FW_SERIAL	0x06

void packet_received(void);
void send_ack(unsigned char);
void send_packet(void);
void send_state(void);
void forward_state(unsigned char, unsigned char);
