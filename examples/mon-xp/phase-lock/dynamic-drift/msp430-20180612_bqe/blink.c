/*
 * Experiment with external digital clock source
 * (c) 2018, Bruno Quoitin (bruno.quoitin@umons.ac.be)
 */

#include <msp430.h>

void delay()
{
  long int count = 100;
  while (count--);
}

void hw_delay()
{
  /* using Timer_A
  *   select source : TASSELx = ACLK (b01)
   *   select pre-scaler : IDx = 1 (b00)
   *   select mode : MCx = count-up (b01)
   *   clear interrupt flag : TAIFG = 0
   */
  TACTL = TASSEL_1 | ID_0 | MC_1 | TACLR ;

  /* set initial value */
  TACCR0 = 1000;

  while ( ! (TACTL & TAIFG) );
}

int main()
{
  /* Disable watchdog timer */
  WDTCTL = WDTPW | WDTHOLD;

  /* Configure GPIO
     - P1.0 output -> ACLK
     - P1.1 output -> LF error LED
     - P1.2 output -> blinking LED
     - P1.3 output -> oscillator error LED
     - P1.4 output -> SMCLK
   */
  P1DIR |= 0x1F;

  /* According to family datasheet, section 5.2.3,
     the LF oscillator can be disabled and the XIN input
     can take an external clock of frequency between .4 and 16 MHz

     Conditions to be met
    - OSCOFF = 0 (enables LF osc.)
    - LFXT1Sx (BCSCTL3) = 11
    - XCAPs (BCSCTL3) = 00 (lowest cap values)
  */
  /* use an external clock (that is not a crystal) on XIN/XOUT */
  _BIC_SR(OSCOFF);
  BCSCTL3 |= LFXT1S_3;
  //BCSCTL3 &= ~LFXT1S_3;
  BCSCTL3 &= ~( XCAP1 | XCAP0 ); /* internal capacity = 1 pF */

  /* select LF as MCLK source */
  BCSCTL2 |= SELM_3; /* Select LF osc. as MCLK source */
  //BCSCTL2 |= DIVM_3; /* Divide MCLK by 8 */
  BCSCTL2 &= ~DIVM_3;
  BCSCTL2 |= SELS;

  /* use peripheral module function on
     PIN P1.0 and P1.4 */
  P1SEL |= 0x01; /* output ACLK through P1.0 */
  P1SEL |= 0x10; /* output SMCLK through P1.4 */


  while (1) {
    /* blink LED */
    P1OUT |= 0x04;
    hw_delay();
    P1OUT &= ~0x04;
    hw_delay();

    /* clear oscillator fault
       (this flag does not auto-clear)
     */
    IFG1 &= ~OFIFG;

    /* LF osc. fault is detected if no clock signal
       is provided externally or if its frequency
       is below some threshold
     */
    if (BCSCTL3 & LFXT1OF) {
      P1OUT |= 0x02;
    } else {
      P1OUT &= ~0x02;
    }

    /* oscillator fault
     */
    if (IFG1 & OFIFG) {
      P1OUT |= 0x08;
    } else {
      P1OUT &= ~0x08;
    }
  }
}

