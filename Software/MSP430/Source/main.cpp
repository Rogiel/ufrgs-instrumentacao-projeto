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

#define SENSOR_LVL BIT5
#define SENSOR_REFLVL BIT3
#define SENSOR_ENVREF BIT4

#define ALL_SENSORS SENSOR_LVL | SENSOR_REFLVL | SENSOR_ENVREF

typedef uint64_t counter_t;

struct measurement_t {
	counter_t time;
	counter_t counter;

	measurement_t() {
		time = 0;
		counter = 0;
	}
};

struct context_t {
	measurement_t level;
	measurement_t reference;
	measurement_t environment;
};

context_t context;
counter_t counter = 0;

uint8_t lastState = 0;
bool hasChange = false;

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	P1DIR |= BIT0;
	P2DIR |= BIT0;

	P1DIR &= ~(ALL_SENSORS);
	P1IE |=  ALL_SENSORS;              // P1.3 interrupt enabled
	P1IES |= ALL_SENSORS;              // P1.3 Hi/lo edge
	P1REN |= ALL_SENSORS;              // Enable Pull Up on SW2 (P1.3)
	P1IFG &= ~(ALL_SENSORS);           // P1.3 IFG cleared

	TACCTL0 = CCIE;
	TACTL = TASSEL_2 + MC_2 + ID_0; // Set the timer A to SMCLCK, Continuous

	uart_init();

	__enable_interrupt();
	__bis_SR_register(GIE);

	while(true) {
		packet_t packet;
		packet.payload.sync = 0xBF;
		packet.payload.level = 16000000.0 / context.level.counter;
		packet.payload.reference = 16000000.0 / context.reference.counter;
		packet.payload.environment = 16000000.0 / context.environment.counter;

		for(int i = 0; i < sizeof(packet.payload); i++) {
			uart_putc(packet.buffer[i]);
			uart_wait_put();
		}
	}
//	__bis_SR_register(LPM0_bits + GIE);

	return 0;
}

//// Timer A0 interrupt service routine
static void __attribute__((__interrupt__(TIMER0_A0_VECTOR))) TimerA0 (void) {
	counter++;
	P2OUT ^= BIT0;
}

static counter_t counter_time() {
	return TA0R | (counter << 16);
}

static void processValue(measurement_t& measurement, uint8_t bit);

// Port1 interrupt service routine
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port1 (void) {
#define SENSOR_PROCESSING(name, id)									\
	if((port & name) != (lastState & name) && (port & name) == 0) 	\
		processValue(context.id, name);

	uint8_t port = P1IN;

	SENSOR_PROCESSING(SENSOR_LVL, level);
	SENSOR_PROCESSING(SENSOR_REFLVL, reference);
	SENSOR_PROCESSING(SENSOR_ENVREF, environment);

	lastState = port;
}

static void processValue(measurement_t& measurement, uint8_t bit) {
	P1IE &= ~bit;

	counter_t now = counter_time();
	counter_t delta = now - measurement.time;

	if(delta >= 1600) {
		measurement.time = now;
		measurement.counter = delta;
	}

	P1IE |= bit;
	P1IFG &= ~bit;
}