/*
 *  Note: this code works for a circuit where white, green, yellow and red LEDs
 *  are connected to Arduino pins 8, 10, 11, 12 respectively. This is to leave
 *  Arduino pin 9 for the PWM output signal
 * 
 */

#include <avr/interrupt.h> 

uint8_t lightState = 1;

ISR(TIMER1_COMPA_vect) {

  /* change lightState every 1 second */
  if (lightState == 3) {
    lightState = 1; 
  } else {
    lightState++;
  }
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
   
   // Configure timer to CTC mode 
   TCCR1A = 0;
   TCCR1B = 0;
   TCCR1B |= (1<<WGM12);


   // Configure the timer prescaler to 1024
   TCCR1B |= (1<<CS10);
   TCCR1B |= (1<<CS12);

   OCR1A = 15625;         // setting the compare value/overflow value
   TIMSK1 |= (1<<OCIE1A); // enabling the compare ISR()

   sei();   // enable all interrupts after configuration is complete


   /*
    *   Superloop
    * 
    */

    while (1) {
      asm volatile ("nop");
    }
 }
