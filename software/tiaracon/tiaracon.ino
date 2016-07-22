
/*
Yet Another ATTiny-based Jar of Fireflies
Joe FitzPatrick, @securelyfitz, joefitz@securinghardware.com

Software, hardware, and build notes at https://git.io/fireflies

Arduino sketch for ATTiny45+ based jar of fireflies. 6 charlieplexed LEDs plus a capacitive button
Based very heavily on Jason Webb's jar of fireflies:

todos:
--be more consistent with globals vs. passed values
--clean up state change code
--fix 1 vs 2 led option
--switch power modes when in state 0 to prolong standby battery life https://learn.sparkfun.com/tutorials/h2ohno/low-power-attiny
--turn off ADC to save power



Jason Webb - zen.webb@gmail.com
Project wiki: http://jason-webb.info/wiki/index.php?title=Jar_of_Fireflies
Github repo: https://github.com/jasonwebb/Jar-of-Fireflies
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

void setup() {
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  sei();                                  // enables interrupts
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // everything off

  pinMode(3,INPUT_PULLUP);
/*  // flash each LED to show they work and so we know we just put in a good battery
  do{
    for(int j=0; j<6; j++) {
      turnOn(j);
//      delay(250);
    }
  }while (currmode=0);
  randomSeed(millis());*/
}

ISR(PCINT0_vect)
{
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
    case 4:   //random fade slow
      randomSeed(millis());
      currled=random(0,6);
      for (int brightness=0;brightness<256;brightness++){
        analogOn(currled, brightness);
        delay(6);
      }
      for (int brightness=255;brightness>0;brightness--){
        analogOn(currled, brightness);
        delay(6);
      }
      break;
    case 5:
      turnOffAll(); 
      sleep_enable();
      sleep_cpu();
    default:
      currmode=0;
      break;
  }
}

//useful test function
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
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT);
}
// Write an analog value to an LED.
void analogOn(byte led, byte value) {
  switch(led) {
    case 4:
      pinMode(pinC, INPUT);
      digitalWrite(pinA, LOW);
      pinMode(pinA, OUTPUT);
      analogWrite(pinB, value);
      pinMode(pinB, OUTPUT);
      break;
    case 3:
      pinMode(pinC, INPUT);
      digitalWrite(pinB, LOW);
      pinMode(pinB, OUTPUT);
      analogWrite(pinA, value);
      pinMode(pinA, OUTPUT);
      break;
    case 5:
      pinMode(pinA, INPUT);
      analogWrite(pinB, 255-value);
      pinMode(pinB, OUTPUT);
      digitalWrite(pinC, HIGH);
      pinMode(pinC, OUTPUT);
      break;
    case 2:
      pinMode(pinA, INPUT);
      digitalWrite(pinC, LOW);
      pinMode(pinC, OUTPUT);
      analogWrite(pinB, value);
      pinMode(pinB, OUTPUT);
      break;
    case 1:
      pinMode(pinB, INPUT);
      analogWrite(pinA, 255-value);
      pinMode(pinA, OUTPUT);
      digitalWrite(pinC, HIGH);
      pinMode(pinC, OUTPUT);
      break;
    case 0:
      pinMode(pinB, INPUT);
      digitalWrite(pinC, LOW);
      pinMode(pinC, OUTPUT);
      analogWrite(pinA, value);
      pinMode(pinA, OUTPUT);      
      break;
  }
}
