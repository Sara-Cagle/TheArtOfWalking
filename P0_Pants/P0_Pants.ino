/**
* "The Art of Walking" (Fancy Pants) by Sara Cagle
* Initially designed for HCDE 539, Physical Prototyping under instruction of Adi Azulay.
* December 2021
* University of Washington
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

#define buttonPin 6
#define buzzerPin 13
#define LxInput A0
#define LyInput A1
#define LzInput A2
#define RxInput A3
#define RyInput A4
#define RzInput A5
#define LLEDPin 9
#define RLEDPin 10
#define LED_COUNT 30
#define sampleSize 25
#define ringHigh 700 /* Highest tone for the buzzer to ring*/
#define ringLow 500 /* Lowest tone for the buzzer to ring*/
#define numOfRings 3
#define ringTime 200
#define walkAlottedTime 500
#define blurAmount 2

bool pantsOn = false;
CRGB ledsL[LED_COUNT];
CRGB ledsR[LED_COUNT];

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  /* Initialize the neopixels using FastLED*/
  FastLED.addLeds<NEOPIXEL, LLEDPin>(ledsL, LED_COUNT);
  FastLED.addLeds<NEOPIXEL, RLEDPin>(ledsR, LED_COUNT);
  FastLED.setBrightness(50);
  for(int blur = 0; blur<blurAmount; blur++){
    blur1d(ledsL, LED_COUNT, 64); /* Keep value between 50 and 172 for performance purposes*/
    blur1d(ledsR, LED_COUNT, 64);
  }
  

  /* Test to confirm strands are working */
  Serial.println("Checking left and right strands. Should be red for 5 seconds.");
  for(int testIndex = 0; testIndex<LED_COUNT;testIndex++){
    ledsL[testIndex] = CHSV(0, 255, 255);
    ledsR[testIndex] = CHSV(0, 255, 255);
  }
  FastLED.show();
  delay(5000);
  resetLights();
  Serial.println("Test completed. Looping now beginning.");
  delay(20);
}

bool leftIsWalking = false;
bool rightIsWalking = false;
unsigned long leftMotionTimeTaken = 0;
unsigned long rightMotionTimeTaken = 0;

void loop() {
  
  if(buttonPressed()){
    togglePantsOnOff();
  }
  if(pantsOn){
      leftIsWalking = checkAccelerometer(true);
      rightIsWalking = checkAccelerometer(false);
      if(leftIsWalking){
        leftMotionTimeTaken = millis();
        if(millis()-leftMotionTimeTaken<walkAlottedTime) {   
          makeWaterfall(true);
        } else {
          /* don't trigger waterfall, user is still in a single step */
        }
      }
      if(rightIsWalking){
        rightMotionTimeTaken = millis();
        if(millis()-rightMotionTimeTaken<walkAlottedTime) {   
          makeWaterfall(false);
        } else {
          /* don't trigger waterfall, user is still in a single step */
        }
      }
  }
  delay(10); /* Helps code run smoother*/
}

/*Waterfall lighting variables*/
  int waterfallLeftCounter = 0; /* goes from 0 to 12 */
  int extraL = 0; /* Variables for further light management*/
  int extraL1 = 0;
  int extraL2 = 0;
  int waterfallRightCounter = 0; /* goes from 0 to 12 */
  int extraR = 0;
  int extraR1 = 0;
  int extraR2 = 0;
/**
* Creates a light-up waterfall effect for the given leg. The lights turn off when done.
* bool isLeft: true indicates the left leg and false indicates the right leg
*/
void makeWaterfall(bool isLeft) {
  if(isLeft){
    if(waterfallLeftCounter > 12 ) {
      waterfallLeftCounter = 0; /* waterfall already completed, reset to 0 */
    }
    switch(waterfallLeftCounter) { /* This section of code can be improved heavily and will be the starting point of my next iteration.*/
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
        ledsL[waterfallLeftCounter]=CHSV(160, random8(), random8(100,255));
        break;
      case 5:
      case 6:
      case 7:
        /* Neopixels index: 5,20,6,19,7,18*/
        if(waterfallLeftCounter==5){
          extraL = 20;
        }
        if(waterfallLeftCounter==6){
          extraL=19;
        }
        if(waterfallLeftCounter==7){
          extraL=18;
        }
        ledsL[waterfallLeftCounter]=CHSV(160, random8(), random8(100,255));
        ledsL[extraL]=CHSV(160, random8(), random8(100,255));
        break;
      case 8:
        /* Neopixels index: 8,17,21 */
        ledsL[waterfallLeftCounter]=CHSV(160, random8(), random8(100,255));
        ledsL[17]=CHSV(160, random8(), random8(100,255));
        ledsL[21]=CHSV(160, random8(), random8(100,255));
        break;
      case 9:
      case 10:
      case 11:
      case 12:
        /* Neopixels index: 9,16,22,29,10,15,23,28,11,14,24,27,12,13,25,26 */
        if(waterfallLeftCounter==9){
          extraL = 16;
          extraL1 = 22;
          extraL2 = 29;
        }
        if(waterfallLeftCounter==10){
          extraL = 15;
          extraL1 = 23;
          extraL2 = 28;
        }
        if(waterfallLeftCounter==11){
          extraL = 14;
          extraL1 = 24;
          extraL2 = 27;
        }
        if(waterfallLeftCounter==12){
          extraL = 13;
          extraL1 = 25;
          extraL2 = 26;
        }
        ledsL[waterfallLeftCounter]=CHSV(160, random8(), random8(100,255));
        ledsL[extraL]=CHSV(160, random8(), random8(100,255));
        ledsL[extraL1]=CHSV(160, random8(), random8(100,255));
        ledsL[extraL2]=CHSV(160, random8(), random8(100,255));
        break;
    }
    waterfallLeftCounter++;
    FastLED.show();
    delay(10);

  } else {
    if(waterfallRightCounter > 12 ) {
      waterfallRightCounter = 0; /* waterfall already completed, reset to 0 */
    }
    switch(waterfallRightCounter) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
        ledsR[waterfallRightCounter]=CHSV(160, random8(), random8(100,255));
        break;
      case 5:
      case 6:
      case 7:
        /* Neopixels index: 5,20,6,19,7,18*/
        if(waterfallRightCounter==5){
          extraR = 20;
        }
        if(waterfallRightCounter==6){
          extraR=19;
        }
        if(waterfallRightCounter==7){
          extraR=18;
        }
        ledsR[waterfallRightCounter]=CHSV(160, random8(), random8(100,255));
        ledsR[extraR]=CHSV(160, random8(), random8(100,255));
        break;
      case 8:
        /* Neopixels index: 8,17,21 */
        ledsR[waterfallRightCounter]=CHSV(160, random8(), random8(100,255));
        ledsR[17]=CHSV(160, random8(), random8(100,255));
        ledsR[21]=CHSV(160, random8(), random8(100,255));
        break;
      case 9:
      case 10:
      case 11:
      case 12:
        /* Neopixels index: 9,16,22,29,10,15,23,28,11,14,24,27,12,13,25,26 */
        if(waterfallRightCounter==9){
          extraR = 16;
          extraR1 = 22;
          extraR2 = 29;
        }
        if(waterfallRightCounter==10){
          extraR = 15;
          extraR1 = 23;
          extraR2 = 28;
        }
        if(waterfallRightCounter==11){
          extraR = 14;
          extraR1 = 24;
          extraR2 = 27;
        }
        if(waterfallRightCounter==12){
          extraR = 13;
          extraR1 = 25;
          extraR2 = 26;
        }
        ledsR[waterfallRightCounter]=CHSV(160, random8(), random8(100,255));
        ledsR[extraR]=CHSV(160, random8(), random8(100,255));
        ledsR[extraR1]=CHSV(160, random8(), random8(100,255));
        ledsR[extraR2]=CHSV(160, random8(), random8(100,255));
        break;
    }
    waterfallRightCounter++;
    FastLED.show();
    delay(10);
  }
  fadeToBlackBy(ledsL, LED_COUNT, 60);
  fadeToBlackBy(ledsR, LED_COUNT, 60);
}

/* Accelerometer variables */
  int LxRaw;
  int LyRaw;
  int LzRaw;
  int RxRaw;
  int RyRaw;
  int RzRaw;
  int lastLXRaw = 0;
  int lastLYRaw = 0;
  int lastLZRaw = 0;
  int lastRXRaw = 0;
  int lastRYRaw = 0;
  int lastRZRaw = 0;
  bool walkDetected = false;
/**
* Checks the given accelerometer to see if the leg is stepping or not
* bool isLeft indicates checking the left leg accelerometer (true) or the right leg (false)
* returns true if the accelerometer has detected motion for the given leg, false otherwise
*/
bool checkAccelerometer(bool isLeft){
  walkDetected = false;
  if(isLeft){ /*Left leg*/
    LxRaw = readAxis(LxInput);
    LyRaw = readAxis(LyInput);
    LzRaw = readAxis(LzInput);
    if(crossedThreshhold(LxRaw, LyRaw, LzRaw, lastLXRaw, lastLYRaw, lastLZRaw)) {
      walkDetected = true;
    }
    lastLXRaw = LxRaw;
    lastLYRaw = LyRaw;
    lastLZRaw = LzRaw;
  } else { /* Right leg*/
    RxRaw = readAxis(RxInput);
    RyRaw = readAxis(RyInput);
    RzRaw = readAxis(RzInput);
    if(crossedThreshhold(RxRaw, RyRaw, RzRaw, lastRXRaw, lastRYRaw, lastRZRaw)) {
      walkDetected = true;
    }
    lastRXRaw = RxRaw;
    lastRYRaw = RyRaw;
    lastRZRaw = RzRaw;
  }
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
* Toggles the pants off and on, changing the current state.
* Will beep correspondingly and print to Serial.
*/
void togglePantsOnOff(){
  pantsOn = !pantsOn;
  if(pantsOn){
    Serial.println("Pants are ON");
  } else {
    resetLights();
    Serial.println("Pants are OFF");
  }
  ringBuzzer(pantsOn);
}

/**
* Resets all of the lights by turning them off and 
* all associated variables.
*/
void resetLights() {
  FastLED.clear();
  FastLED.show();
  waterfallLeftCounter = 0;
  extraL = 0;
  extraL1 = 0;
  extraL2 = 0;
  waterfallRightCounter = 0;
  extraR = 0;
  extraR1 = 0;
  extraR2 = 0;
}

/*Buzzer variables */
  int ringInterval = (ringHigh-ringLow)/(numOfRings-1);
  int i = 0;
/**
* Rings the buzzer,  either in an upward tone or a downward tone.
* bool ringUp: ring upward or ring downard in tone
*/
void ringBuzzer(bool ringUp){
  if(ringUp){
    for(i = 0; i< numOfRings; i++){
      tone(buzzerPin, ringLow+i*ringInterval, ringTime); 
      delay(ringTime);
    }
  } else{
    for(i = 0; i< numOfRings; i++){
      tone(buzzerPin, ringHigh-i*ringInterval, ringTime); 
      delay(ringTime);
    }
  }
}

/*Button variables*/
  int buttonState;
  int lastButtonState;
/**
* Checks if the button is pressed.
* Returns true if pressed, false otherwise.
*/
bool buttonPressed(){
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      Serial.println("Button pressed");
      return true;
    }
    lastButtonState = buttonState;
  }
  return false;
}