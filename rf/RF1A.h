/* -------------------------------------------------------------------------
 *                                Defines
 * -------------------------------------------------------------------------
 */

/* Define whether or not to check for RF1AERROR flags during debug */ 
//#define DEBUG 1              // For debug of error flags ONLY


#define CONF_REG_SIZE	 47	   /* There are 47 8-bit configuration registers */ 

void delay_RF(volatile unsigned long p);
unsigned char Strobe(unsigned char strobe);
unsigned char ReadSingleReg(unsigned char addr);
void WriteSingleReg(unsigned char addr, unsigned char value);
void ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void ResetRadioCore (void);
void WritePATable(void);
void rf_transmit(unsigned char *buffer, unsigned char length);
void ReceiveOn(void);
void ReceiveOff(void);
void WriteSmartRFReg(const unsigned char SmartRFSetting[][2], unsigned char size);
