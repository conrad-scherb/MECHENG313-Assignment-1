#include <util/delay.h>
 
volatile int systemState = 1;
volatile int configMode = 0;
volatile uint16_t conversionResult;
 
int main(void) {
  Serial.begin(9600);
  Serial.println("start");
 
  // Set the LED pins to output
  DDRB |= (1<<DDB0); // White
  DDRB |= (1<<DDB1); // Green
  DDRB |= (1<<DDB2); // Yellow
  DDRB |= (1<<DDB3); // Red
 
  // Set input pins
  DDRD &= ~(1<<DDD2);
 
  // Set interrupt for configuration mode change
  EIMSK |= (1<<INT0);
  EICRA |= (1<<ISC01)|(1<<ISC00);
 
  // Set ADC interrupt
  ADMUX = 0x0;          // input pin ADC0
  ADMUX |= (1<<REFS0); // V_REF = AV_cc
 
  ADCSRA = 0x0;
  ADCSRA |= (1<<ADEN)|(1<<ADIE);    // enable the ADC and enable the conversion complete interrupt 
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS2);
 
  sei();
 
  // Setup variable to track current on light 
  // (G: 0, B: 1, R: 2)
  int ledState = 0;
 
  // Turn on the white LED
  PORTB  |= (1<<PORTB0); 
 
  int analogPin = 0;
 
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
    conversionResult = 0;
    ADCSRA |= (1<<ADSC);    // Now start an ADC conversion 
    for (int i = 0; i < systemState; i++) {
      PORTB |= (1<<PORTB0);
      variableDelay(500/systemState);
      PORTB &= ~(1<<PORTB0); 
      variableDelay(500/systemState);
    }
    _delay_ms(2000);
  }
}
 
void variableDelay(int ms) {
  for (int i = 0; i < ms; i++) {
    _delay_ms(1);
  }
} 
 
 
ISR(INT0_vect) {
  configMode == 0 ? configMode = 1 : configMode = 0;
}
 
ISR(ADC_vect) {
  conversionResult = ADC;
  if (conversionResult < 255) {
    systemState = 1;
  } else if (conversionResult < 512) {
    systemState = 2;
  } else if (conversionResult < 767) {
    systemState = 3;
  } else {
    systemState = 4;
  }
  Serial.println(systemState);
}