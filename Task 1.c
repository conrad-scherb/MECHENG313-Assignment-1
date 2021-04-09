#include <util/delay.h>

int main(void) {
  // Set the LED pins to output
  DDRB |= (1<<DDB1); // Green
  DDRB |= (1<<DDB2); // Yellow
  DDRB |= (1<<DDB3); // Red
  
  // Setup variable to track current on light 
  // (G: 0, B: 1, R: 2)
  int ledState = 0;
  
  while (1) {
    if (ledState == 0) {
      PORTB &= ~(1<<PORTB3);
      PORTB |= (1<<PORTB1);
    } else if (ledState == 1) {
      PORTB &= ~(1<<PORTB1);
      PORTB |= (1<<PORTB2);
    } else {
      PORTB &= ~(1<<PORTB2);
      PORTB |= (1<<PORTB3);
    }
    
    // Wait 1 second, then advance to next state
    _delay_ms(1000);
    ledState = ledState == 2 ? 0 : ledState + 1;
  }
}