#include  <msp430g2553.h>

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;
    P1DIR |= 0x01;

    for (;;) {
        P1OUT ^= 0x01;
        for(unsigned int i=0; i< 20000; i++);
    }
}