#include <msp430.h>
#include <intrinsics.h>

#include "RF1A.h"
#include "PMM.h"
#include "RF_Connection.h"

// RF Configuration
#include "smartrf_CC430F5137_868MHz_1k2Baud.h"

const unsigned char RF1A_REGISTER_CONFIG[CONF_REG_SIZE]=
{
  SMARTRF_SETTING_IOCFG2  ,  // IOCFG2: GDO2 signals on RF_RDYn     
  SMARTRF_SETTING_IOCFG1  ,  // IOCFG1: GDO1 signals on RSSI_VALID     
  SMARTRF_SETTING_IOCFG0  ,  // IOCFG0: GDO0 signals on PA power down signal to control RX/TX switch         
  SMARTRF_SETTING_FIFOTHR , // FIFOTHR: RX/TX FIFO Threshold: 33 bytes in TX, 32 bytes in RX    
  SMARTRF_SETTING_SYNC1   , // SYNC1: high byte of Sync Word
  SMARTRF_SETTING_SYNC0   , // SYNC0: low byte of Sync Word
  SMARTRF_SETTING_PKTLEN  , // PKTLEN: Packet Length in fixed mode, Maximum Length in variable-length mode      
  SMARTRF_SETTING_PKTCTRL1, // PKTCTRL1: No status bytes appended to the packet    
  SMARTRF_SETTING_PKTCTRL0, // PKTCTRL0: Fixed-Length Mode, No CRC       
  SMARTRF_SETTING_ADDR    , // ADDR: Address for packet filtration       
  SMARTRF_SETTING_CHANNR  , // CHANNR: 8-bit channel number, freq = base freq + CHANNR * channel spacing          
  SMARTRF_SETTING_FSCTRL1 , // FSCTRL1: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio) 
  SMARTRF_SETTING_FSCTRL0 , // FSCTRL0: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio) 
  SMARTRF_SETTING_FREQ2   , // FREQ2: base frequency, high byte      
  SMARTRF_SETTING_FREQ1   , // FREQ1: base frequency, middle byte      
  SMARTRF_SETTING_FREQ0   , // FREQ0: base frequency, low byte      
  SMARTRF_SETTING_MDMCFG4 , // MDMCFG4: modem configuration (refer to User's Guide/SmartRF Studio)     
  SMARTRF_SETTING_MDMCFG3 , // MDMCFG3:           "                      "
  SMARTRF_SETTING_MDMCFG2 , // MDMCFG2:           "                      "  
  SMARTRF_SETTING_MDMCFG1 , // MDMCFG1:           "                      "  
  SMARTRF_SETTING_MDMCFG0 , // MDMCFG0:           "                      "
  SMARTRF_SETTING_DEVIATN , // DEVIATN: modem deviation setting (refer to User's Guide/SmartRF Studio)         
  SMARTRF_SETTING_MCSM2   , // MCSM2: Main Radio Control State Machine Conf. : timeout for sync word search disabled      
  SMARTRF_SETTING_MCSM1   , // MCSM1: CCA signals when RSSI below threshold, stay in RX after packet has been received      
  SMARTRF_SETTING_MCSM0   , // MCSM0: Auto-calibrate when going from IDLE to RX or TX (or FSTXON )      
  SMARTRF_SETTING_FOCCFG  , // FOCCFG: Frequency Offset Compensation Conf.  
  SMARTRF_SETTING_BSCFG   , // BSCFG: Bit Synchronization Conf.       
  SMARTRF_SETTING_AGCCTRL2, // AGCCTRL2: AGC Control   
  SMARTRF_SETTING_AGCCTRL1, // AGCCTRL1:     "   
  SMARTRF_SETTING_AGCCTRL0, // AGCCTRL0:     "   
  SMARTRF_SETTING_WOREVT1 , // WOREVT1: High Byte Event0 Timeout    
  SMARTRF_SETTING_WOREVT0 , // WOREVT0: High Byte Event0 Timeout
  SMARTRF_SETTING_WORCTRL , // WORCTL: Wave On Radio Control ****Feature unavailable in PG0.6****
  SMARTRF_SETTING_FREND1  , // FREND1: Front End RX Conf.    
  SMARTRF_SETTING_FREND0  , // FREND0: Front End TX Conf.               
  SMARTRF_SETTING_FSCAL3  , // FSCAL3: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)    
  SMARTRF_SETTING_FSCAL2  , // FSCAL2:              "      
  SMARTRF_SETTING_FSCAL1  , // FSCAL1:              "     
  SMARTRF_SETTING_FSCAL0  , // FSCAL0:              "     
  0x00                    , // Reserved *read as 0*
  0x00                    , // Reserved *read as 0*
  SMARTRF_SETTING_FSTEST  , // FSTEST: For test only, irrelevant for normal use case
  SMARTRF_SETTING_PTEST   , // PTEST: For test only, irrelevant for normal use case
  SMARTRF_SETTING_AGCTEST , // AGCTEST: For test only, irrelevant for normal use case
  SMARTRF_SETTING_TEST2   , // TEST2  : For test only, irrelevant for normal use case    
  SMARTRF_SETTING_TEST1   , // TEST1  : For test only, irrelevant for normal use case
  SMARTRF_SETTING_TEST0     // TEST0  : For test only, irrelevant for normal use case       
};



/**********************************************************************/
/*  Function name: rf_init                                            */
/*  	Parameters                                                    */
/*          Input   :  No       			                          */
/*          Output  :  No                                             */
/*	Action: Initialize RF radio core.                             	  */
/**********************************************************************/
void rf_init(void){
  
  // Increase PMMCOREV level to 2 in order to avoid low voltage error 
  // when the RF core is enabled
  SetVCore(2);
  ResetRadioCore();   

  // For WOR 
	PMMCTL0_H = 0xA5;
	PMMCTL0_L |= PMMHPMRE_L;
	PMMCTL0_H = 0x00;

   WriteBurstReg(IOCFG2, (unsigned char*)RF1A_REGISTER_CONFIG, CONF_REG_SIZE);
   WritePATable();
}

