//***************************************************************************************
// MSP430 Timer Blink LED Demo - Timer A Software Toggle P1.0 & P1.6
//
// Description; Toggle P1.0 and P1.6 by xor'ing them inside of a software loop.
// Since the clock is running at 1Mhz, an overflow counter will count to 8 and then toggle
// the LED. This way the LED toggles every 0.5s.
// ACLK = n/a, MCLK = SMCLK = default DCO
//
// MSP430G2xx
// -----------------
// /|\| XIN|-
// | | |
// --|RST XOUT|-
// | P1.6|-->LED
// | P1.0|-->LED
//
// Aldo Briano
// Texas Instruments, Inc
// June 2010
// Built with Code Composer Studio v4
//***************************************************************************************
#include <msp430g2553.h>
#include "uart.h"

#include <stdint.h>

#include "packet.h"

void uart_rx_isr(unsigned char c) {
}

bool running = false;
bool sending = false;

typedef uint32_t counter_t;
counter_t counter = 0;

uint32_t start = 0;
uint32_t end = 0;

uint8_t portKnownState;

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	P1DIR |= BIT0;

	P2IE |=  BIT0;              // P1.3 interrupt enabled
	P2IES |= BIT0;              // P1.3 Hi/lo edge
	P2REN |= BIT0;              // Enable Pull Up on SW2 (P1.3)
	P2IFG &= ~(BIT0);           // P1.3 IFG cleared

	CCTL0 = CCIE;
	TACTL = TASSEL_2 + MC_2 + ID_0; // Set the timer A to SMCLCK, Continuous

	uart_init();

	__enable_interrupt();
	__bis_SR_register(LPM0_bits + GIE);

	return 0;
}

//// Timer A0 interrupt service routine
static void __attribute__((__interrupt__(TIMER0_A0_VECTOR))) Timer0 (void) {
	__disable_interrupt();
	counter++;
	__enable_interrupt();
}

static uint32_t counter_time() {
	return TA0R | (counter << 16);
}

// Timer A0 interrupt service routine
static void __attribute__((__interrupt__(PORT2_VECTOR))) Port2 (void) {
	if(running) {
		P2IE &= ~BIT0;

		running = false;
		end = counter_time();

		packet_t packet;
		packet.payload.sync = 0xBF;
		packet.payload.integer = end - start;

		for(int i = 0; i < sizeof(packet.payload); i++) {
			uart_putc(packet.buffer[i]);
			uart_wait_put();
		}

		P1OUT ^= BIT0;                            // P1.0 = toggle
		P2IE |= BIT0;
	} else {
		start = counter_time();
		running = true;
	}

	P2IFG &= ~BIT0;                           // P1.3 IFG cleared
}