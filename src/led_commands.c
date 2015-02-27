#include <msp430.h>
#include <stdint.h>

#define PWM_PERIOD 255

// Functions that set and stop the LEDs using the PWM

// Setting the LEDs with the PWM
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

// Stopping the LEDs with the PWM
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
