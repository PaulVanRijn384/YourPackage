/*
rainbow.h
DGIF-6037 301
based on components of Adafruit "no wait" strandtest projects
includes a separate routine to manage the colours of the buttons
*/
#define TLED_PIN    21
#define TLED_COUNT 26

#define BLED_COUNT 4 // number of lights in the button
#define BLED_START 30 // position in the strip
#define BUTTON_GAP 125 // milliseconds for button flashing

// constants for blinking light show

Adafruit_NeoPixel strip(TLED_COUNT, TLED_PIN, NEO_GRB + NEO_KHZ800);

// globals for the marquee led show
unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 3800;   // Pattern Interval (ms)
bool          patternComplete = false;

int           pixelInterval = 70;       // Pixel Interval (ms)
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle
uint16_t      pixelNumber = TLED_COUNT - 6;  // Total Number of Pixels
// button globals
uint32_t  buttonColour = 0;
unsigned long buttonNextChange = 0;
int buttonFlip = 0;
uint16_t buttonStartPixel = pixelNumber + 1;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/*
 Button lights
 
 */
void buttonLights(int buttonMode) {
  unsigned long buttonMillis = millis(); 
  if (buttonNextChange < buttonMillis) { // time to make a change
      buttonNextChange = buttonMillis + BUTTON_GAP;
    if (buttonMode == 1) { // going up
      buttonColour = strip.Color(0, 0, 220); //Blue
    } else if (buttonMode == 2) { // going down
      buttonColour = strip.Color(0, 220, 0); //red
    } else { // stopped 
     buttonColour = strip.Color(127, 127, 127); //white
    }
    if (buttonFlip == 0) { // nasty flash
      strip.fill(strip.Color(0, 0, 0), buttonStartPixel, BLED_COUNT); // paint it black
      buttonFlip = 1;
    } else {
      strip.fill((buttonColour), buttonStartPixel, BLED_COUNT); // paint it
      buttonFlip = 0;
    }
    strip.show();
  }
}// button lights
// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  static uint16_t current_pixel = 0;
  pixelInterval = wait;                        //  Update delay time
  strip.setPixelColor(current_pixel++, color); //  Set pixel's color (in RAM)
  strip.show();                                //  Update strip to match
  if(current_pixel >= pixelNumber) {           //  Loop the pattern from the first LED
    current_pixel = 0;
    patternComplete = true;
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  static uint32_t loop_count = 0;
  static uint16_t current_pixel = 0;

  pixelInterval = wait;                   //  Update delay time

  strip.clear();

  for(int c=current_pixel; c < pixelNumber; c += 3) {
    strip.setPixelColor(c, color);
  }
  strip.show();

  current_pixel++;
  if (current_pixel >= 3) {
    current_pixel = 0;
    loop_count++;
  }

  if (loop_count >= 10) {
    current_pixel = 0;
    loop_count = 0;
    patternComplete = true;
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   
  for(uint16_t i=0; i < pixelNumber; i++) {
    strip.setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
  }
  strip.show();                             //  Update strip to match
  pixelCycle++;                             //  Advance current cycle
  if(pixelCycle >= 256)
    pixelCycle = 0;                         //  Loop the cycle back to the begining
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time  
  for(int i=0; i < pixelNumber; i+=3) {
    strip.setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time  
  }
  strip.show();
  for(int i=0; i < pixelNumber; i+=3) {
    strip.setPixelColor(i + pixelQueue, strip.Color(0, 0, 0)); //  Update delay time  
  }      
  pixelQueue++;                           //  Advance current queue  
  pixelCycle++;                           //  Advance current cycle
  if(pixelQueue >= 3)
    pixelQueue = 0;                       //  Loop
  if(pixelCycle >= 256)
    pixelCycle = 0;                       //  Loop
}

