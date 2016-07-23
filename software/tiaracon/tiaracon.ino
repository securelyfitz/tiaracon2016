  
/*
ATTiny code for 2016 Tiaracon Badge
Joe FitzPatrick, @securelyfitz, joefitz@securinghardware.com

Software, hardware, and build notes at https://git.io/tiaracon

Arduino sketch for ATTiny45+ based tiaracon badge. 6 charlieplexed LEDs plus a button
Derived from https://git.io/fireflies which in turn relied heavily on Jason Webb's jar of fireflies:
Github repo: https://github.com/jasonwebb/Jar-of-Fireflies

todos:
DONE--be more consistent with globals vs. passed values
DONE--clean up state change code
DONE--fix 1 vs 2 led option
DONE--switch power modes when in state 0 to prolong standby battery life https://learn.sparkfun.com/tutorials/h2ohno/low-power-attiny
DONE--turn off ADC to save power
DONE--tweak charlieplexing code to mininmize glitches and flashes
--fix PWM code to minimize glitches and flashes.


*/
#include <avr/sleep.h>
#define F_CPU 1000000


// Pin assignments
const byte pinA = 0;
const byte pinB = 1;
const byte pinC = 2;
#define NUMSTATES 6;

int currmode=0;
bool pressed= false;
int currled=0;
unsigned long startmillis=0;
int elapsedmillis=0;

void setup() {
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  sei();                                  // enables interrupts
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // everything off

  pinMode(3,INPUT_PULLUP);  //pullup so it's normally high, active low button
}

ISR(PCINT0_vect)
{
  random(6);//adds entropy because its called for every event, even debounced ones
  // if we're not in the pressed state and you press the button, enter the pressed state and increment mode
  if (!digitalRead(3)&&!pressed){pressed=true;currmode++;}
  // if we're in the pressed state, and the button is now released, leave pressed state
  if (digitalRead(3)&&pressed){pressed=false;}
}

void loop() {  
  switch(currmode){
    case 0:    //all on
      for(int j=0; j<6; j++) {
        turnOn(j);
      }
      break;
    case 1:    //I on
      turnOn(5);
      break;
    case 2:    //tiara on
      for(int j=0; j<5; j++) {
        turnOn(j);
      }
      break;
    case 3:   //ordered fade quick
      currled++;
      currled%=6;
      for (int brightness=0;brightness<256;brightness++){
        analogOn(currled, brightness);
      }
      for (int brightness=255;brightness>0;brightness--){
        analogOn(currled, brightness);
      }
      break;
    case 4:   //random fade slow. Set a counter, and calculate LED PWM based on that counter
      //if elapsed time exceeded - pick a led, start a counter
      elapsedmillis=millis()-startmillis;   // calculate elapsed time so we know what to do
      if (startmillis==0){     //this should only hit the very first time we get to this case
        startmillis=millis();
      }else if (elapsedmillis>=4600){   //timeout, choose a new LED and reset our start itme
        startmillis+=elapsedmillis;
        currled=random(6);  
        analogOn(currled,0);
      }else if (elapsedmillis>=4088){   //lights out time
        turnOffAll();
      }else if (elapsedmillis>=2048){   //fade out time
        analogOn(currled,(-8-elapsedmillis)/8);      
      }else{                            //fade in time 
        analogOn(currled,(elapsedmillis)/8);
      }
      break;
    case 5:  // low power mode
      turnOffAll(); 
      //wait until button is released before going to sleep, otherwise we'll just wake back up.
      while(!digitalRead(3)){delay(100);}
      sleep_enable();
      sleep_cpu();
    default: // if we get here by mistake go back to zero.
      currmode=0;
      break;
  }
}

//useful test function to output a value
void flash(int count){
  for(int i=0;i<=count;i++){
    turnOn(1);
    delay(500);
    turnOff(1);
    delay(500);
  }
}

// Turn on a single LED to full brightness
void turnOn(byte led) {
  analogOn(led, 255);
}

// Turn off an LED completely
void turnOff(byte led) {
  analogOn(led, 0);
}

//turns off all LEDs.
void turnOffAll() {
  //go to high impedence mode- disconnect all active power
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT);
  //put everything low, so that no power is supplied when they go to output mode
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, LOW);
  //clear all pwm setttings, analogWrite calls pinMode(output) before setting the output value!!
  analogWrite(pinA, 0);
  analogWrite(pinB, 0);
  analogWrite(pinC, 0);
  //by now, all pins should be 0v outputs without accidentally turning any on
}
// Write an analog value to an LED.
void analogOn(byte led, byte value) {
//  if (value<8){value=0;}
//  if (value>240){value=255;}
  switch(led) {
    case 4:
      pinMode(pinC, INPUT);
      
      digitalWrite(pinA, LOW);
      pinMode(pinA, OUTPUT);
      
      analogWrite(pinB, value);
      break;
    case 3:
      pinMode(pinC, INPUT);

      digitalWrite(pinB, LOW);
      pinMode(pinB, OUTPUT);
      
      analogWrite(pinA, value);
      break;
    case 5:
      pinMode(pinC, INPUT); // to avoid PinB->PinC current
      pinMode(pinA, INPUT);
      
      analogWrite(pinB, 255-value);
      
      digitalWrite(pinC, HIGH);
      pinMode(pinC, OUTPUT);
      break;
    case 2:
      pinMode(pinA, INPUT);
      
      digitalWrite(pinC, LOW);
      pinMode(pinC, OUTPUT);

      analogWrite(pinB, value);
      break;
    case 1:
      pinMode(pinC, INPUT); // to avoid PinA->PinC current
      pinMode(pinB, INPUT);
      
      analogWrite(pinA, 255-value);

      digitalWrite(pinC, HIGH);
      pinMode(pinC, OUTPUT);
      break;
    case 0:
      pinMode(pinB, INPUT);
      
      digitalWrite(pinC, LOW);
      pinMode(pinC, OUTPUT);

      analogWrite(pinA, value);
      break;
  }
}
