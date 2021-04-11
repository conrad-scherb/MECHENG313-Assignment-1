#include <util/delay.h>

#define GREEN 1<<DDB1
#define RED 1<<DDB3
#define RED_ON PORTB |= (1<<PORTB3)
#define GREEN_ON PORTB |= (1<<PORTB1)
#define CONFIG_OUTPUT(arg1, arg2) (DDRB |= (arg1)|(arg2))

volatile int lb1Breached, lb2Breached = 0;

int main(void) {
  Serial.begin(9600);
  Serial.println("Start");

  CONFIG_OUTPUT(GREEN, RED);
   
  // Set interrupts
  EIMSK |= (1<<INT1)|(1<<INT0);
  EICRA |= (1<<ISC11)|(1<<ISC01);
 
  sei();
  
  while (1) {
    PORTB |= lb1Breached*(1<<PORTB3)|lb2Breached*(1<<PORTB1);
    _delay_ms(100);
    PORTB = 0;
    _delay_ms(100);
  }
}

// LB1 (LEFT)
ISR(INT1_vect) {
  lb1Breached = 1;
}

// LB2 (RIGHT)
ISR(INT0_vect) {
  lb2Breached = 1;
}