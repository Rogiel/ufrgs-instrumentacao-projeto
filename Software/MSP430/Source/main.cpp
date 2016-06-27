#include <msp430g2553.h>
#include "uart.h"

#include <stdint.h>

#include "packet.h"

#define SENSOR_LVL BIT5
#define SENSOR_REFLVL BIT3
#define SENSOR_ENVREF BIT4

#define ALL_SENSORS SENSOR_LVL | SENSOR_REFLVL | SENSOR_ENVREF

typedef uint64_t counter_t;

struct measurement_ctx {
	counter_t time;
	counter_t counter;

	measurement_ctx() {
		time = 0;
		counter = 0;
	}
};

struct context_t {
	measurement_ctx level;
	measurement_ctx reference;
	measurement_ctx environment;
};

context_t context;
counter_t counter = 0;

uint8_t lastState = 0;

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

		__disable_interrupt();
		context_t contextCopy = context;
		__enable_interrupt();

		packet.payload.level.id = 0;
		packet.payload.level.cycleCount = contextCopy.level.counter;

		packet.payload.reference.id = 1;
		packet.payload.reference.cycleCount = contextCopy.reference.counter;

		packet.payload.environment.id = 2;
		packet.payload.environment.cycleCount = contextCopy.environment.counter;

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
}

static counter_t counter_time() {
    // shifts the incrementing counter by 16 bits to the left
    // and appends the 16-bit timer to the result
	return TA0R | ((counter & 0x0000FFFF) << 16);
}

static void processValue(measurement_ctx& measurement, uint8_t bit);

// Port1 interrupt service routine
static void __attribute__((__interrupt__(PORT1_VECTOR))) Port1 (void) {
    if(P1IFG & SENSOR_LVL) {
        processValue(context.level, SENSOR_LVL);
    } else if(P1IFG & SENSOR_REFLVL) {
        processValue(context.reference, SENSOR_REFLVL);
    } else if(P1IFG & SENSOR_ENVREF) {
        processValue(context.environment, SENSOR_ENVREF);
    }
}

static void processValue(measurement_ctx& measurement, uint8_t bit) {
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