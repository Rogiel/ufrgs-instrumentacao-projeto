/*
 * This file is part of the MSP430 hardware UART example.
 *
 * Copyright (C) 2012 Stefan Wendler <sw@kaltpost.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
 * Hardware UART example for MSP430.
 *
 * Stefan Wendler
 * sw@kaltpost.de
 * http://gpio.kaltpost.de
 ******************************************************************************/

#include <msp430.h>
#include <legacymsp430.h>

#include <math.h>

#include "uart.h"

/**
 * Receive Data (RXD) at P1.1
 */
#define RXD		BIT1

/**
 * Transmit Data (TXD) at P1.2
 */
#define TXD		BIT2

/**
 * Callback handler for receive
 */
void (*uart_rx_isr_ptr)(unsigned char c);

// Index math is BAUDRATE_xxxx + CPUCLOCK_xxMHZ
unsigned int cpuclock_baudrate_to_UCA_BR[] = {
        104, 833, 1250, 1666, // 9600 baud for 1mhz, 8mhz, 12mhz, 16mhz
        52, 416,  625,  833, // 19200 baud for 1mhz, 8mhz, 12mhz, 16mhz
        26, 208,  312,  416, // 38400 baud for 1mhz, 8mhz, 12mhz, 16mhz
        17, 142,  214,  285, // 56000 baud for 1mhz, 8mhz, 12mhz, 16mhz
        8,  69,  104,  138, // 115200 baud for 1mhz, 8mhz, 12mhz, 16mhz
        7,  62,   93,  125, // 128000 baud for 1mhz, 8mhz, 12mhz, 16mhz
        3,  31,   46,   62, // 256000 baud for 1mhz, 8mhz, 12mhz, 16mhz
};
// Index math is BAUDRATE_xxxx + CPUCLOCK_xxMHZ
// Generated with http://mspgcc.sourceforge.net/cgi-bin/msp-uart.pl?clock=1000000&baud=9600&submit=calculate
unsigned char cpuclock_baudrate_to_MCTL[] = {
        UCBRS_1, UCBRS_2, UCBRS_0, UCBRS_6,
        UCBRS_0, UCBRS_6, UCBRS_0, UCBRS_2,
        UCBRS_0, UCBRS_3, UCBRS_4, UCBRS_6,
        UCBRS_7, UCBRS_7, UCBRS_2, UCBRS_6,
        UCBRS_6, UCBRS_4, UCBRS_1, UCBRS_7,
        UCBRS_7, UCBRS_4, UCBRS_6, UCBRS_0,
        UCBRS_7, UCBRS_2, UCBRS_7, UCBRS_4,
};

#define CPUCLOCK_COUNT 4
#define CPUCLOCK_1MHZ 0
#define CPUCLOCK_8MHZ 1
#define CPUCLOCK_12MHZ 2
#define CPUCLOCK_16MHZ 3

// The baudrates are multiples of CPUCLOCK_COUNT so a simple add is the index in the pseudo-multi-dim-arrays.
#define BAUDRATE_COUNT 7
#define BAUDRATE_9600 0
#define BAUDRATE_19200 4
#define BAUDRATE_38400 8
#define BAUDRATE_56000 12
#define BAUDRATE_115200 16
#define BAUDRATE_128000 20
#define BAUDRATE_256000 24

void uart_init(void) {
    uart_set_rx_isr_ptr(0L);

    P1SEL  = RXD + TXD;
    P1SEL2 = RXD + TXD;
    UCA0CTL1 = UCSSEL_2;                     // SMCLK

    // Set the baudrate dividers and modulation
    unsigned int N_div;
    N_div = 16000000 / 9600;

    float N_div_f;
    N_div_f = (float)16000000 / (float)9600;

    if(N_div >= 16) {
        // We can use Oversampling mode
        N_div /= 16;
        UCA0BR0 = (N_div & 0x00FF);
        UCA0BR1 = ((N_div & 0xFF00) >> 8);

        N_div_f /= 16.0;
        UCA0MCTL = (unsigned char)(((N_div_f) - round(N_div_f))*16.0f) << 4; // Set BRF
        UCA0MCTL |= UCOS16; // Enable Oversampling Mode
    } else {
        // We must use the Low Frequency mode
        UCA0BR0 = (N_div & 0x00FF);
        UCA0BR1 = ((N_div & 0xFF00) >> 8);

        UCA0MCTL = (unsigned char)((N_div_f - round(N_div_f))*8.0f) << 1; // Set BRS
    }

    UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void uart_set_rx_isr_ptr(void (*isr_ptr)(unsigned char c)) {
    uart_rx_isr_ptr = isr_ptr;
}

unsigned char uart_getc() {
    while (!(IFG2&UCA0RXIFG));                // USCI_A0 RX buffer ready?
    return UCA0RXBUF;
}

void uart_putc(unsigned char c) {
    UCA0TXBUF = c;                    		// TX
}

void uart_wait_put() {
	while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
}


void uart_puts(const char *str) {
    while(*str) {
		uart_putc(*str++);
		uart_wait_put();
	}
}

interrupt(USCIAB0RX_VECTOR) USCI0RX_ISR(void) {
    if(uart_rx_isr_ptr != 0L) {
        (uart_rx_isr_ptr)(UCA0RXBUF);
    }
}