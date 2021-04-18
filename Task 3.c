#include <util/delay.h>

#define GREEN 1<<DDB1
#define RED 1<<DDB3
#define OSC 1<<DDB5
#define RED_ON PORTB |= (1<<PORTB3)
#define GREEN_ON PORTB |= (1<<PORTB1)
#define CONFIG_OUTPUT(arg1, arg2, arg3) (DDRB |= (arg1)|(arg2)|(arg3))

volatile int elapsed = -2;
volatile int lb1Breach = 0;
volatile int lb2Breach = 0;

int main(void) {
  Serial.begin(9600);
  Serial.println("Start");

  CONFIG_OUTPUT(GREEN, RED, OSC);
   
  // Set interrupts
  EIMSK |= (1<<INT1)|(1<<INT0);
  EICRA |= (1<<ISC11)|(1<<ISC01);
  
  OCR1A = 12499;
  TCCR1B |= (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
  
  while (1) {
  }
}

void blink(int target) {
  PORTB |= (1<<target);
  _delay_ms(100);
  PORTB &= ~(1<<target);
}
/*
void handleBreach(int breachPos) {
  breaches++;
  blink(breachPos);
}*/


// LB1 (LEFT)
ISR(INT1_vect) {
  blink(PORTB3);
  TCCR1B |= (1 << CS12);
}

// LB2 (RIGHT)
ISR(INT0_vect) {
  blink(PORTB1);
  Serial.print(elapsed);
  elapsed = -2;
  TCCR1B &= ~(1 << CS12);
}

ISR(TIMER1_COMPA_vect) {
  elapsed++;
}
/*
#include <util/delay.h>

#define GREEN 1<<DDB1
#define RED 1<<DDB3
#define OSC 1<<DDB5
#define RED_ON PORTB |= (1<<PORTB3)
#define GREEN_ON PORTB |= (1<<PORTB1)
#define CONFIG_OUTPUT(arg1, arg2, arg3) (DDRB |= (arg1)|(arg2)|(arg3))

volatile int breaches = 0;

int main(void) {
  Serial.begin(9600);
  Serial.println("Start");

  CONFIG_OUTPUT(GREEN, RED, OSC);
   
  // Set interrupts
  EIMSK |= (1<<INT1)|(1<<INT0);
  EICRA |= (1<<ISC11)|(1<<ISC01);
  
  // Timer setting
  TCCR0A = 0;
  TCCR0B = 0;
  TCCR0B |= (1<<CS02);
  TIMSK0 |= (1<<TOIE0); 
 
  sei();
  
  while (1) {
  }
}

void blink(int target) {
  PORTB |= (1<<target);
  _delay_ms(100);
  PORTB &= ~(1<<target);
}

void handleBreach(int breachPos) {
  breaches++;
  blink(breachPos);
}

// LB1 (LEFT)
ISR(INT1_vect) {
  handleBreach(PORTB3);
}

// LB2 (RIGHT)
ISR(INT0_vect) {
  handleBreach(PORTB1);
}

ISR(TIMER0_OVF_VECT) {
  Serial.println("Yep!");
}*/

/*
volatile int elapsed = 0;

int main(void)
{
  	Serial.begin(9600);
    OCR1A = 12499;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << CS12);
    sei();


    while (1);
    {
        // we have a working Timer
    }
}
ISR (TIMER1_COMPA_vect) {
  	elapsed++;
}

*/