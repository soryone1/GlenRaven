/*
  This code shows the way using touch sense fabric to controll the color change fabric,
  and they are one piece together.

  The boards I used is an Adafruit Pro Trinket 5v, Power boost buck.

  Create by Jasper Wang at Glen Raven

  08/25/2019
*/

#include <CapPin.h>
#define LED 12                // for test only

CapPin cPin_5 = CapPin(A4);   // this pin is connected to touch sense pin

// variables for touch sense

float smoothed;                         // smooth the cap reading figure
int pressed;
int buttonState = LOW;
int lastButtonState = LOW;
int lastPressedTime = 0;
bool turnState = false;                // a flag to indicate the time to invert state
int sensorValue;
const int debounceDelay = 300;         // the time to get a solid finger touch, low the number if want to react faster
int threshold = 100;                   // this is the threshold for capacity touch sense, adjust the number to make it stable
unsigned long indicatorTime = 0;       // this is a time to track the touch sense light

// variables for strips

unsigned long previousMillis = 0;
unsigned long shiftInterval = 15000;     // the time to shift to the next strip
unsigned long fadeInterval = 2000;       // the time before turn off the last strip, create a over laid fading effect
bool startToFade = false;                // a flag to indicate the time to start caculate the fadeInterval.
unsigned long fadeMilis = 0;

const byte stripPins[] = {3, 4, 5, 6, 9, 11, 12, A2};   // change the strips pins here, better an order, from top to bottom
const byte numbersOfStrips = sizeof(stripPins) / sizeof(stripPins[0]);   // get the number of the strips

byte index = 0;                             // the index of the strips: starts from 0 to numberOfStrips - 1

/***********************  all setup here  **************************************************/

void setup() {
  Serial.begin(115200);
  Serial.println("start");               // a sign to indicate the program has started

  pinMode(LED, OUTPUT);                  // LED for test only

  for (byte i = 0; i < numbersOfStrips; i++)    // setup pinModes for strips
  {
    pinMode(stripPins[i], OUTPUT);
  }

}

/***********************  a function to turn off all the strips  ****************************/

void allStripsOff() {
  for (byte j = 0; j < numbersOfStrips; j++) {

    digitalWrite(stripPins[j], LOW);                      // set all the strips to low
  }
}

/***********************  a function to shift the strips  ***********************************/

void stripsShift(unsigned long thisTime) {               // an argurement to get the real time

  digitalWrite(stripPins[index], HIGH);                  // set the first strip on -- index[0]

  //  Serial.print(index);                                   // print which one is on
  //  Serial.println(" is on");

  unsigned long currentMillis = thisTime;
  if (currentMillis - previousMillis > shiftInterval)
  {
    index++;                                             // each time shfit to one strip
    if (index >= numbersOfStrips) {                      // numbersOfStrips
      index = 0;                                         // set back to 0
    }

    startToFade = true;                                  // start to slowly fade out
    previousMillis = currentMillis;
  }

  if (startToFade) {                                     // a function to do fade
    fadeMilis = millis();                                // get the time
    startToFade = false;                                 // set the flag back to false
  }

  if (millis() - fadeMilis > fadeInterval) {              // if after the fadeInterval, start to fade

    if (index > 0 ) {                                     // fade from index[0] (No.1 strip) to the last 2 strip
      //      Serial.print("start to fade ");
      //      Serial.println(index - 1);
      digitalWrite(stripPins[index - 1], LOW);

    } else {                                               // start to fade the last strip
      //      Serial.println("start to fade 7");
      digitalWrite(stripPins[numbersOfStrips - 1], LOW);
    }
  }
}

/***********************  a function to smooth the data  ***********************************************/

int smooth(int data, float filterVal, float smoothedVal) {                    // simple lowpass filter                                                                              // requires recycling the output in the "smoothedVal" param

  if (filterVal > 1) {                                                        // check to make sure param's are within range
    filterVal = .999999;
  }
  else if (filterVal <= 0) {
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;                                                     // return the value
}

/***********************  a function to control the strip  ***********************************************/

void stripsControl() {

  if (turnState) {                                     // if ready to invert the state
    unsigned long timeNow = millis();                  // grab the current time
    stripsShift(timeNow);                              // put it into the function
  } else {
    allStripsOff();                                    // turn off all the strips
  }

}

/***********************  the main loop is here  ***********************************************/

void loop() {
  long start = millis();
  long total =  cPin_5.readPin(2000);

  // simple lowpass filter to take out some of the jitter
  // change parameter (0 is min, .99 is max) or eliminate to suit
  smoothed = smooth(total, .8, smoothed);

  if (millis() > 2000) {                                                // wait 2 seconds for the initialization
    if ( millis() - start > 5) {                                        // a slightly delay

      Serial.println((int) smoothed);                                   // print the smoothed the result

      if ((int) smoothed > threshold) {                                 // get 2 value states here, pressed or not
        pressed = HIGH;
        digitalWrite(13, HIGH);                                         // turn on the light on Pin 13 to indicate a solid press
        indicatorTime = millis();
      } else {
        pressed = LOW;
      }

      if (millis() - indicatorTime > 2000) {
        digitalWrite(13, LOW);                                           // turn it off after 2 seconds
      }

      sensorValue = pressed;                                             // get the result of the input 0/1

      if (sensorValue != lastButtonState) {                              // if input changes
        lastPressedTime = millis();                                      // grab the time
      }

      if (millis() - lastPressedTime > debounceDelay ) {                  // wait a certain time for a certain touch
        if (sensorValue != buttonState) {
          buttonState = sensorValue;
          if (buttonState == HIGH) {                                      // only active when touched, remove the finger causes nothing
            // Serial.println("touched");
            turnState = ! turnState;
            // here to invert the strips' state
          }
        }

      }

      lastButtonState = sensorValue;                                      // always set update the lastButtonState
    }
  }

  stripsControl();                                                         // control the strips here

}
