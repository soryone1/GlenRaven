/*
 The test code for programming the color change fabric
 Loop through the 8 strips
 By:Jasper Wang 
 08/12/2019 
*/
   
unsigned long previousMillis = 0;
unsigned long shiftInterval = 6000;

const byte stripPins[] = {8,6,5,4};
const byte numbersOfStrips = sizeof(stripPins) / sizeof(stripPins[0]);

byte counter = 0;

void setup()
{
    for (byte i = 0; i < numbersOfStrips; i++)
    {
        pinMode(stripPins[i], OUTPUT);
    }
}

void loop()
{
    unsigned long currentMillis = millis();

    digitalWrite(stripPins[counter], HIGH);

    if (currentMillis - previousMillis >= shiftInterval)
    {
        digitalWrite(stripPins[counter], LOW);
        counter++;

        if (counter > numbersOfStrips)
            counter = 0;

        previousMillis = currentMillis;
    }
}
