/**
 * PANTS VERSION 2 - "The Art of Walking Better" by Sara Cagle
 * 
* Prototype of pants that light up when the wearer is walking. Pants have NeoPixel strips custom sized to each pantleg 
* and a pocket to hold the Arduino components and power source. Leggings worn underneath the pants have one accelerometer
* per leg that detects the walking motion for each leg. (Leggings not required but allow for optimal motion sensing.)
* A push button toggles the pants off/on and a Piezo buzzer chimes to inform the state.
*
* This code is my own save for small snippets from Scott Marley's FastLED tutorial: https://github.com/s-marley/FastLED-basics
* and some bare-bone accelerometer code from Last Minute Engineer's accelerometer tutorial: https://lastminuteengineers.com/adxl335-accelerometer-arduino-tutorial/
*/

#include <FastLED.h>
#include <Wire.h>

// #define buttonPin 6
// #define buzzerPin 13
/*
 * pins for single leg*/
#define xInput 9
#define yInput 7 /* 7 in code but connect to pin 6*/
#define zInput 12
#define LEDPin 10

#define LED_COUNT 30
#define sampleSize 25
#define walkAlottedTime 500
#define blurAmount 2

// #define ringHigh 700 /* Highest tone for the buzzer to ring*/
// #define ringLow 500 /* Lowest tone for the buzzer to ring*/
// #define numOfRings 3
// #define ringTime 200

bool pantsOn = true;
CRGB leds[LED_COUNT]; /*Single leg variable*/

void setup() {
  Serial.begin(9600);
  // pinMode(buzzerPin, OUTPUT);
  // pinMode(buttonPin, INPUT);

  /* Initialize the neopixels using FastLED*/
  FastLED.addLeds<NEOPIXEL, LEDPin>(leds, LED_COUNT);
  FastLED.setBrightness(50);
  for(int blur = 0; blur<blurAmount; blur++){
    blur1d(leds, LED_COUNT, 64); /* Keep value between 50 and 172 for performance purposes*/
  }
  

  /* Test to confirm strands are working */
  Serial.println("Checking strands. Should be red for 5 seconds.");
  for(int testIndex = 0; testIndex<LED_COUNT;testIndex++){
    leds[testIndex] = CHSV(0, 255, 255);
  }
  FastLED.show();
  delay(5000);
  resetLights();
  Serial.println("Test completed. Looping now beginning.");
  delay(20);
}

unsigned long motionTimeTaken = 0;

void loop() {
  
  // if(buttonPressed()){
  //   togglePantsOnOff();
  // }
  if(pantsOn){
      if(checkAccelerometerOneLeg()){/* Check if walking for a single leg*/
        motionTimeTaken = millis();
        if(millis()-motionTimeTaken<walkAlottedTime) {   
          makeWaterfall();
        } else {
          /* don't trigger a new waterfall because user is still in a single step, but need to make sure the waterfall finishes*/
        }
      }
      /*********************************/
      
  }
  delay(10); /* Helps code run smoother*/
}

/** Waterfall lighting variables
* Columns of neopixels, all in a single array, but are toggled individually
**/
  int col0 = 0; /* goes from 0 to 12, the largest stack of lights*/
  int col1 = 0; /* goes from 13 to 20 */
  int col2 = 0; /* goes from 21 to 25 */
  int col3 = 0; /* goes from 26 to 29 */

/**
* Creates a light-up waterfall effect for a single leg. The lights turn off when done.
**/
void makeWaterfall() {
    if(col0 > 12 ) {
      col0 = 0; /* waterfall already completed, reset to 0 */
    }
    /*
    This could be written as a 2D array to represent the different columns of lights, but since a large part of the array would be empty
    since the lights are mostly on the bottom, it's better performance to keep this as a switch case that then checks for lights as the column counter increases.
    */
    switch(col0) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
        leds[col0]=CHSV(160, random8(), random8(100,255));
        break;
      case 5:
      case 6:
      case 7:
        /* Neopixels index: 5,20,6,19,7,18*/
        if(col0==5){
          col1 = 20;
        }
        if(col0==6){
          col1=19;
        }
        if(col0==7){
          col1=18;
        }
        leds[col0]=CHSV(160, random8(), random8(100,255));
        leds[col1]=CHSV(160, random8(), random8(100,255));
        break;
      case 8:
        /* Neopixels index: 8,17,21 */
        leds[col0]=CHSV(160, random8(), random8(100,255));
        leds[17]=CHSV(160, random8(), random8(100,255));
        leds[21]=CHSV(160, random8(), random8(100,255));
        break;
      case 9:
      case 10:
      case 11:
      case 12:
        /* Neopixels index: 9,16,22,29,10,15,23,28,11,14,24,27,12,13,25,26 */
        if(col0==9){
          col1 = 16;
          col2 = 22;
          col3 = 29;
        }
        if(col0==10){
          col1 = 15;
          col2 = 23;
          col3 = 28;
        }
        if(col0==11){
          col1 = 14;
          col2 = 24;
          col3 = 27;
        }
        if(col0==12){
          col1 = 13;
          col2 = 25;
          col3 = 26;
        }
        leds[col0]=CHSV(160, random8(), random8(100,255));
        leds[col1]=CHSV(160, random8(), random8(100,255));
        leds[col2]=CHSV(160, random8(), random8(100,255));
        leds[col3]=CHSV(160, random8(), random8(100,255));
        break;
    }
    col0++;
    FastLED.show();
    delay(10);
    fadeToBlackBy(leds, LED_COUNT, 60);
}


/* Accelerometer variables */
  bool walkDetected = false;
  /* Variables for V2 single leg components*/
  int xRaw;
  int yRaw;
  int zRaw;
  int lastXRaw = 0;
  int lastYRaw = 0;
  int lastZRaw = 0;
/**
 * Checks the accelerometer for a single leg of components.
 * returns a boolean, is the leg detected to be walking or not
 */
bool checkAccelerometerOneLeg() {
  walkDetected = false;
  xRaw = readAxis(xInput);
  yRaw = readAxis(yInput);
  zRaw = readAxis(zInput);
  /* Could look at removing this function for single leg components*/
  if(crossedThreshhold(xRaw, yRaw, zRaw, lastXRaw, lastYRaw, lastZRaw)) {
    walkDetected = true;
  }
  lastXRaw = xRaw;
  lastYRaw = yRaw;
  lastZRaw = zRaw;
  return walkDetected;
}
/*Read axis variables*/
  long reading;
  int j;
/**
* Reads the axis for the given pin. Returns the avergage of the sample.
* int axisPin: the pin indicating X,Y,Z and which leg
* returns the average reading for the pin.
*/
int readAxis(int axisPin){
	reading = 0; 
	delay(1);
	for (j = 0; j < sampleSize; j++){
	  reading += analogRead(axisPin);
	}
	return reading/sampleSize;
}

/* Threshold variables*/
  int threshhold = 75;
  bool crossed = false;
/* The threshold detection will also be improved in the next iteration*/
/**
* Detects if the given values have crossed the threshhold of movement.
* Rather than returning true immediately, it is broken into multiple if statements for debugging.
* int currX, currY, currZ: the current raw values from the accelerometer
* int lastX, lastY, lastZ: the previous raw values from the accelerometer
* return bool, true if the threshold has been crossed, false otherwise
*/
bool crossedThreshhold(int currX, int currY, int currZ, int lastX, int lastY, int lastZ){
  crossed = false;
  if(abs(currX-lastX) > threshhold){
    crossed = true;
  }
  if(!crossed && abs(currY-lastY) > threshhold){
    crossed = true;
  }
  if(!crossed && abs(currZ-lastZ) > threshhold){
    crossed = true;
  }
  return crossed;
}

/**
* Resets all of the lights by turning them off and 
* all associated variables.
*/
void resetLights() {
  FastLED.clear();
  FastLED.show();
  col0 = 0;
  col1 = 0;
  col2 = 0;
  col3 = 0;
}

// /**
// * Toggles the pants off and on, changing the current state.
// * Will beep correspondingly and print to Serial.
// */
// void togglePantsOnOff(){
//   pantsOn = !pantsOn;
//   if(pantsOn){
//     Serial.println("Pants are ON");
//   } else {
//     resetLights();
//     Serial.println("Pants are OFF");
//   }
//   ringBuzzer(pantsOn);
// }

// /*Buzzer variables */
//   int ringInterval = (ringHigh-ringLow)/(numOfRings-1);
//   int i = 0;
// /**
// * Rings the buzzer,  either in an upward tone or a downward tone.
// * bool ringUp: ring upward or ring downard in tone
// */
// void ringBuzzer(bool ringUp){
//   if(ringUp){
//     for(i = 0; i< numOfRings; i++){
//       tone(buzzerPin, ringLow+i*ringInterval, ringTime); 
//       delay(ringTime);
//     }
//   } else{
//     for(i = 0; i< numOfRings; i++){
//       tone(buzzerPin, ringHigh-i*ringInterval, ringTime); 
//       delay(ringTime);
//     }
//   }
// }

// /*Button variables*/
//   int buttonState;
//   int lastButtonState;
// /**
// * Checks if the button is pressed.
// * Returns true if pressed, false otherwise.
// */
// bool buttonPressed(){
//   buttonState = digitalRead(buttonPin);
//   if (buttonState != lastButtonState) {
//     if (buttonState == HIGH) {
//       Serial.println("Button pressed");
//       return true;
//     }
//     lastButtonState = buttonState;
//   }
//   return false;
// }
