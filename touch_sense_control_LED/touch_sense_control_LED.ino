#include <CapPin.h>
#define LED 12


CapPin cPin_5 = CapPin(A1);   // read pin A1

float smoothed;
int pressed;
int buttonState = LOW;
int lastButtonState = LOW;
int lastPressedTime = 0;
bool turnState = false;
int sensorValue;
const int debounceDelay = 300;

void setup()
{

  Serial.begin(115200);
  Serial.println("start");
  pinMode(LED, OUTPUT);


}

void loop()
{


  long start = millis();
  long total =  cPin_5.readPin(2000);


  // simple lowpass filter to take out some of the jitter
  // change parameter (0 is min, .99 is max) or eliminate to suit
  smoothed = smooth(total, .8, smoothed);

  if (millis() > 2000) {                                                // wait 2 seconds for the initialization
    if ( millis() - start > 5) {                                        // a slightly delay

      Serial.println((int) smoothed);                                   // smoothed the result

      if ((int) smoothed > 100) {
        pressed = HIGH;
      } else {
        pressed = LOW;
      }

      sensorValue = pressed;                                             // get the result of the input 0/1

      if (sensorValue != lastButtonState) {                              // if input changes
        lastPressedTime = millis();                                      // grab the time
      }

      if (millis() - lastPressedTime > debounceDelay ) {                  // wait a certain time 
        if (sensorValue != buttonState) {                                 // 
          buttonState = sensorValue;
          if (buttonState == HIGH) {
            //    Serial.println("touched");
            turnState = ! turnState;
          }
        }

      }

      lastButtonState = sensorValue;
    }
  }

  if (turnState) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

}


int smooth(int data, float filterVal, float smoothedVal) {                    // simple lowpass filter                                                                              // requires recycling the output in the "smoothedVal" param

  if (filterVal > 1) {                                                        // check to make sure param's are within range
    filterVal = .999999;
  }
  else if (filterVal <= 0) {
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}
