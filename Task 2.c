#include <util/delay.h>
 
volatile int systemState = 1;
volatile int configMode = 0;
volatile unsigned int elapsed = 0;
volatile uint16_t conversionResult;
 
int main(void) {
  // Set the LED pins to output
  DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3);
 
  // Set input pins
  DDRD &= ~(1<<DDD2);

  // Disable interrupts before configuring them
  cli();
 
  // Set interrupt for configuration mode change
  EIMSK |= (1<<INT0);
  EICRA |= (1<<ISC01)|(1<<ISC00);
 
  // Enable the ADC & conversion complete interrupt
  ADMUX = 0x0;   
  ADMUX |= (1<<REFS0);
  ADCSRA = 0x0;
  ADCSRA |= (1<<ADEN)|(1<<ADIE);    
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS2);

  // Setup a timer which triggers an overflow interrupt every 1ms
  OCR1A = 1999;
  TCCR1B |= (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  TCCR1B |= (1 << CS11);
 
  sei();
 
  // Setup variable to track current on light 
  // (G: 0, B: 1, R: 2)
  int ledState = 0;
 
  // Turn on the white LED
  PORTB  |= (1<<PORTB0); 
  
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
      if (configMode) {
        configModeFSM();
      }
    }
 
    variableDelay(1000*systemState);
    ledState = ledState == 2 ? 0 : ledState + 1;
  }
}
 
void configModeFSM() {
  while (configMode) {
    // Start the ADC conversion
    conversionResult = 0;
    ADCSRA |= (1<<ADSC); 

    // Blink the white LED with appropriate duty cycle
    for (int i = 0; i < systemState; i++) {
      PORTB |= (1<<PORTB0);
      variableDelay(500/systemState);
      PORTB &= ~(1<<PORTB0); 
      variableDelay(500/systemState);
    }

    // Wait 2 seconds until repeated (or exit early if
    // configuration mode is exited)
    unsigned int startTime = elapsed;
    while (elapsed < startTime+2000 && configMode) {
      asm volatile ("nop");
    }
  }
}

// Take advantage of the clock to setup a _delay_ms() that can
// accept variable inputs
void variableDelay(int ms) {
  unsigned int startTime = elapsed;
  while (elapsed < startTime+ms) {
    asm volatile ("nop");
  }
} 
 
// Toggle in and out of configuration mode
ISR(INT0_vect) {
  configMode == 0 ? configMode = 1 : configMode = 0;
}

// Convert the ADC result into a systemState
ISR(ADC_vect) {
  conversionResult = ADC;
  if (conversionResult < 255) {
    systemState = 1;
  } else if (conversionResult < 511) {
    systemState = 2;
  } else if (conversionResult < 767) {
    systemState = 3;
  } else {
    systemState = 4;
  }
}

ISR(TIMER1_COMPA_vect) {
  // Tick up the millisecond count on timer overflow
  elapsed++;
}
 