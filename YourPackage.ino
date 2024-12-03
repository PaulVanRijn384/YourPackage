/*
  Your Package 
  DGIF-6037-301 Nickolas Puckett and Kate Hartman
  Single button amazon package to play DFPong

  includes the ble_funtions.h code for BLE connecton to the DFPong game
  and for the CSV controller protocol

  Includes the buzzer_functions.h which, alas, could not be heard outside the 
  box so feedback was provided by changes to the lights under the big button.

  Includes the rainbow.h code which is mostly the Adafruit stran test code to 
  illuminate the marquee and the lights under the big button.
  
  used the muti tap library by Prashant Kumar:
  Push Button Taps
  https://github.com/pk17r/PushButtonTaps
  - Arduino library to read Debounced Push Button Output as Single Tap, Double
    Tap or Long Press without blocking program flow
  - classifies taps as one of 4 types
    - 0 -> noTap
    - 1 -> singleTap
    - 2 -> doubleTap
    - 3 -> longPress
 
*/
#include <Adafruit_NeoPixel.h>
#include <PushButtonTaps.h>
#include "ble_functions.h"

#include "rainbow.h"

// multi button input pin
const int BUTTON_PIN = 12;

const int LED_PIN = LED_BUILTIN; // Status LED pin
const char* deviceName = "paulvr";

// button object 
PushButtonTaps pushBtn;

int tapsRecorded = 0;
int tapTimesRead = 0;
uint16_t firstTapMs;
uint16_t gapBetweenTapsMs;
uint16_t secondTapMs;
// Movement state tracking
int currentMovement = 0;         // Current movement value (0=none, 1=up, 2=down, 3=handshake)

void setup() {
  //Serial.begin(9600);
  //while(!Serial) {
    //delay(10);
  //}
  //delay(500);
  // Initialize Bluetooth Low Energy with device name and status LED
  setupBLE(deviceName, LED_PIN);
  // button pin can be initialized here or directly at pushBtn decleration
  // button pin is Pulled Up and will be set as Active Low
  pushBtn.setButtonPin(BUTTON_PIN);
  // set pu the theatre marquee
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(175); // Set BRIGHTNESS (max = 255)
}

void loop() {
    // Update BLE connection status and handle incoming data
  updateBLE();
  // look for some taps - set the global currentMovement variable
  checkInput();
  // send the state to the game via BLE
  sendMovement(currentMovement);
  // update the theatre lights and the button lights
  doLights();
  delay(1);
}
/*
 marquee lights from the Adafruit strand test
 this is the "no wait" version so that we can keep up with button presses and blinking lights
 */
void doLights(){
  
  buttonLights(currentMovement); // do the button lights - code in rainbow.h

  unsigned long currentMillis = millis();                     //  Update the lights pattern time
  if( patternComplete || (currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
    patternComplete = false;
    patternPrevious = currentMillis;
    patternCurrent++;                                         //  Advance to next pattern
    if(patternCurrent >= 7)
      patternCurrent = 0;
  }

  if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
    pixelPrevious = currentMillis;                            //  Run current frame
    switch (patternCurrent) {
      case 7:
      case 0:
      case 1:
      case 2:
      case 6:
        theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
        break;
      case 5:
        theaterChase(strip.Color(0, 0, 127), 50); // Blue
        break;
      case 3:
        theaterChase(strip.Color(0, 127, 0), 50); // Red
        break;
      case 4:
        theaterChase(strip.Color(127, 127, 127), 50); // White
        break;
    }
  }
} // end do lights

// associate button taps with paddle movements
void checkInput(){
 byte tap = pushBtn.checkButtonStatus();
  if(tap != 0) {
    switch(tap) {
      case 1:
        //Serial.print("SINGLE TAP ");
        currentMovement = 1;
        break;
      case 2:
        //Serial.print("DOUBLE TAP ");
        currentMovement = 2;
        break;
      case 3:
        //Serial.print("LONG PRESS ");
        currentMovement = 0;
        break;
    }
  }
} // inputs
