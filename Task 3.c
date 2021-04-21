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

void pulsePwm(double speed) {
  if (speed > 100) {
    speed = 100;
  }
  Serial.println("Speed: ");
  Serial.print(speed);
  double waitPeriod = 1000-(speed*10);
  Serial.println("Wait Period: ");
  Serial.print(waitPeriod);
  while(1) {
  	PORTB |= (1<<PORTB5); 
    _delay_ms(waitPeriod);
    PORTB &= ~(1<<PORTB5); 
    _delay_ms(1000-waitPeriod);
  }
}

// LB1 (LEFT)
ISR(INT1_vect) {
  blink(PORTB3);
  lb1Breach = 1;
  TCCR1B |= (1 << CS12);
}

// LB2 (RIGHT)
ISR(INT0_vect) {
  if (lb1Breach == 1) {
    blink(PORTB1);
    TCCR1B &= ~(1 << CS12);
    lb1Breach = 0;
    
    // Calculate speed
    double time = (double)elapsed/5;
    double velocity = 20/time;
    Serial.println(velocity);
    pulsePwm(velocity);
    elapsed = -2;
  }
}

ISR(TIMER1_COMPA_vect) {
  elapsed++;
}