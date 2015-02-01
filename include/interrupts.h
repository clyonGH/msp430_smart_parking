#define PACKET_LEN (5) // PACKET_LEN <= 61
#define RSSI_IDX (PACKET_LEN) // Index of appended RSSI
#define CRC_LQI_IDX (PACKET_LEN+1) // Index of appended LQI, checksum
#define CRC_OK (BIT7) // CRC_OK bit
#define PATABLE_VAL (0x51) // 0 dBm output

unsigned char RxBuffer[PACKET_LEN+2];
unsigned char RxBufferLength = 0;
unsigned char transmitting = 0;
unsigned char receiving = 0;

