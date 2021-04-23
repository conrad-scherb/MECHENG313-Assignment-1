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

volatile int lb1Breaches[10];
volatile int lb2Breaches[10];
volatile int elapsedTimes[10];
volatile int lb1idx, lb2idx = 0;

int main(void) {
  Serial.begin(9600);
  Serial.println("Start");

  CONFIG_OUTPUT(GREEN, RED, OSC);
  
  for (int i = 0; i < 10; i++) {
  	lb1Breaches[i] = 0;
  }
  
  cli();
   
  // Set interrupts
  EIMSK |= (1<<INT1)|(1<<INT0);
  EICRA |= (1<<ISC11)|(1<<ISC01);
  
  // Using the timer with OCR1A = 1999 and a prescaler
  // of 8 causes an interrupt every 1ms
  OCR1A = 1999;
  TCCR1B |= (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
  
  while (1) {
    // Sort out the PWM generation
    Serial.print("ok");
    // Find the most recent non-zero elapsed time
    for (int i = 9; i >= 0; i--) {
      if (elapsedTimes[i] != 0) {
      	Serial.println(elapsedTimes[i]);
      }
    }
  }
}

void blink(int target) {
  PORTB |= (1<<target);
  _delay_ms(100);
  PORTB &= ~(1<<target);
}
/*
void pulsePwm(double speed) {
  if (speed > 100) {
    speed = 100;
  }
  double waitPeriod = 1000-(speed*10);
  while(1) {
  	PORTB |= (1<<PORTB5); 
    _delay_ms(waitPeriod);
    PORTB &= ~(1<<PORTB5); 
    _delay_ms(1000-waitPeriod);
  }
}*/

// LB1 (LEFT)
ISR(INT1_vect) {
  blink(PORTB3);
  TCCR1B |= (1 << CS11); // Start the timer with prescaler of 8
  lb1Breaches[lb1idx] = 1;
  lb1idx++;
}

// LB2 (RIGHT)
ISR(INT0_vect) {
  blink(PORTB1);
  lb2Breaches[lb2idx] = 1;
  lb2idx++;
}

ISR(TIMER1_COMPA_vect) {
  for (int i = 0; i < 10; i++) {
    if (lb1Breaches[i] == 1 && lb2Breaches[i] == 0) {
      elapsedTimes[i] = elapsedTimes[i] + 1;
    }
  }
  
  Serial.print("[");
  for (int i = 0; i < 10; i++) {
  	Serial.print(elapsedTimes[i]);
    Serial.print(" ");
  }
  Serial.print("]\n");
}