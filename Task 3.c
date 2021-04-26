#include <util/delay.h>
 
#define GREEN DDB1
#define RED DDB3
#define PWM DDB5

volatile unsigned int elapsed = 0;
volatile double speed = 0;
volatile int startTimes[5] = {0, 0, 0, 0, 0};
volatile int lb1idx = 0;
 
int main(void) {

  // Configure the pins to output for green+red LEDs and PWM
  DDRB != (1<<GREEN)|(1<<RED)|(1<<PWM);

  // Disable all interrupts while configuring them
  cli();
 
  // Set external interrupts
  EIMSK |= (1<<INT1)|(1<<INT0);
  EICRA |= (1<<ISC11)|(1<<ISC01);
 
  // Using the timer with OCR1A = 1999 and a prescaler
  // of 8 causes an interrupt every 1ms
  OCR1A = 1999;
  TCCR1B |= (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  TCCR1B |= (1 << CS11);

  // Enable the interrupts after setting them up
  sei();
 
  while (1) {
    // Produce the PWM signal in the main run loop
    int startTime = elapsed;
    
    // Turn on signal
    PORTB |= (1<<PORTB2);

    // Keep the signal on for speed*10 ms
    while (elapsed < startTime+(int(speed)*10)) {
      asm volatile ("nop");
    }

    // Turn off signal
    PORTB &= ~(1<<PORTB2);

    // Wait until 1 second from startTime recorded
    while (elapsed < startTime+1000) {
      asm volatile ("nop");
    }
  }
}
 
void blink(int target) {
  PORTB |= (1<<target);
  _delay_ms(100);
  PORTB &= ~(1<<target);
}

// LB1 (LEFT)
ISR(INT1_vect) {
  // Record the current ms count into appropriate position
  // in startTimes
  startTimes[lb1idx] = elapsed;
 
  // Change position to write next LB1 press into
  lb1idx++;

  // Flash the red LED
  blink(PORTB3);
}
 
// LB2 (RIGHT)
ISR(INT0_vect) {
  // Make sure there is a car to still pass LB2
  if (startTimes[0] != 0) {
    // Find the elapsed time between the first car to pass 
    // LB1 and the current LB2 press, and calculate speed from it
    double elapsedTime = elapsed-startTimes[0];
   
    // Find the speed to update PWM state 
    speed = ((double)20/elapsedTime)*1000;

    printArray(startTimes);
   
    // Shift the start times array to the left
    for (int i = 0; i < 4; i++) {
        startTimes[i] = startTimes[i+1];
    }
    startTimes[4] = 0;
  
    printArray(startTimes);
   
    // Next time LB1 is pressed, change the position
    // on start times array to write to
    lb1idx--;

    // Flash the green LED
    blink(PORTB1);
  }
}
 
ISR(TIMER1_COMPA_vect) {
  // Tick up the millisecond count on timer overflow
  elapsed++;
}
 