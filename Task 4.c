/*
 *  Note: this code works for a circuit where white, green, yellow and red LEDs
 *  are connected to Arduino pins 8, 10, 11, 12 respectively. This is to leave
 *  Arduino pin 9 for the PWM output signal
 * 
 */

#include <avr/interrupt.h> 

void trafficLight();
void timer0Delay();
uint8_t carCount = 0;
uint8_t ovfCount = 0;
uint8_t delayMode = 0;
uint8_t lightState = 1;



ISR(TIMER1_OVF_vect) {
  
  trafficLight();   // change the trafficLight

  /* change lightState every 1 second */
  if (lightState == 3) {
    lightState = 1; 
  } else {
    lightState++;
  }
}


ISR(TIMER0_COMPA_vect) {

  if (delayMode) {
    ovfCount++;
  }
}


ISR(INT0_vect) {

  // only trigger when traffic light is red  
  if ((PORTB & (~PORTB | (1<<PB4))) != 0) {

    // pulse the white LED twice 
    for (unsigned int i = 0; i < 2; i++) {
      PORTB |= (1<<PB0);
      timer0Delay();
      PORTB &= ~(1<<PB0);
      timer0Delay(); 
    }

    if (carCount < 100) {
      carCount++;             // carCount > 100 has no effect on PWM (remains @ 100% duty-cycle) 
    }
  }
}


void trafficLight() {
  
  if (lightState == 1) {
    PORTB &= ~(1<<PB2);   // red light ON
    PORTB &= ~(1<<PB3);
    PORTB |= (1<<PB4);
    
  } else if (lightState == 2) {
    PORTB |= (1<<PB2);    // green light ON
    PORTB &= ~(1<<PB3);
    PORTB &= ~(1<<PB4);
    
  } else {
    PORTB &= ~(1<<PB2);   // yellow light ON
    PORTB |= (1<<PB3);
    PORTB &= ~(1<<PB4);
    
  }
}



/* This function is equivalent to _delay_ms(125) - useful for pulsing white LED */
void timer0Delay() {

  ovfCount = 0;
  delayMode = 1;
  TCNT0 = 0;
  unsigned int i = 1;

  while (ovfCount < 7) {
    asm volatile ("nop");
  }

  while (TCNT0 < 161) {
    asm volatile ("nop");
  }

  ovfCount = 0;
  delayMode = 0;
}



/*--------------------------------------------------------------------------
 * 
 *                            Main function   
 * 
 --------------------------------------------------------------------------*/

int main(void) {

  /*
   *  Set up
   * 
   */
   
  // Configure traffic light pins to output
  DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4);

  cli();   // disable all interrupts during configuration

/**************     Configure timer0 (8-bit) to CTC mode (mode 14)       *****************/
  TCCR0A = 0;
  TCCR0B = 0;
  TCCR0A |= (1<<WGM01);
  TCCR0B |= (1<<CS00) | (1<<CS02);    // configure prescaler to 1024
  OCR0A = 15625;                      // set the compare value to 1 second
  TIMSK0 |= (1<<OCIE0A);              // enable the compare ISR() 

   
/*************    Configure timer1 (16-bit) to fast PWM mode (mode 14)   *****************/ 
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A |= (1<<WGM11);
  TCCR1B |= (1<<WGM12) | (1<<WGM13);
  TCCR1A |= (1<<COM1A0) | (1<<COM1A1);        // set OC1A on compare match, clear at bottom
  TCCR1B |= (1<<CS10) | (1<<CS12);            // configure the timer prescaler to 1024
  ICR1 = 15625;                               // set the top value for a one second period 
  OCR1A = 15625;                              // default PWM is zero duty-cycle 
  TIMSK1 |= (1<<TOIE1);                       // enabling the overflow ISR()

  sei();   // enable all interrupts after configuration is complete


  /*
   *   Superloop
   * 
   */

  while (1) {
      
    // control PWM duty-cycle 
    OCR1A = (15625 * (1 - (carCount/100)));
  }
 }
