/* Helper functions for an two-dimensional XY matrix of pixels.
  

Writing to and reading from the 'holes' in the layout is 
also allowed; holes retain their data, it's just not displayed.

You can also test to see if you're on- or off- the layout
like this
  if( XY(x,y) > LAST_VISIBLE_LED ) { ...off the layout...}

X and Y bounds checking is also included, so it is safe
to just do this without checking x or y in your code:
  leds[ XY(x,y) ] == CRGB::Red;
All out of bounds coordinates map to the first hidden pixel.
*/

#include <FastLED.h>
#include <JC_Button.h>

#define LED_PIN           7           // Output pin for LEDs
#define MODE_PIN          A7           // Input pin for button to change mode
#define COLOR_ORDER       GRB         // Color order of LED string [GRB]
#define CHIPSET           WS2812B     // LED string type [WS2182B]
#define BRIGHTNESS        10          // Overall brightness [50]
#define LAST_VISIBLE_LED  255         // Last LED that's visible
#define MAX_MILLIAMPS     700        // Max current in mA to draw from supply
#define DEBOUNCE_MS       20          // Number of ms to debounce the button [20]
#define LONG_PRESS        700         // Number of ms to hold the button to count as long press [500]
#define BTN_INVERSE       false        // true -> false interprets a high logic level to mean the button is pressed, true interprets a low level as pressed.
#define PATTERN_TIME      10          // Seconds to show each pattern on autoChange [10]
#define kMatrixWidth      8          // Matrix width
#define kMatrixHeight     32          // Matrix height
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)                                       // Total number of Leds
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)   // Largest dimension of matrix

CRGB leds[ NUM_LEDS ];
uint8_t brightness = BRIGHTNESS;

// Used to check RAM availability. Usage: Serial.println(freeRam());
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Button stuff
uint8_t buttonPushCounter = 9;
uint8_t state = 0;
bool autoChangeVisuals = false;
// Disable internal pull up resistor
Button modeBtn(MODE_PIN, DEBOUNCE_MS, false, BTN_INVERSE);


void incrementButtonPushCounter() {
  buttonPushCounter = (buttonPushCounter + 1) % 10;
}

// Include various patterns
#include "Rainbow.h"
#include "Fire.h"
#include "Squares.h"
#include "Circles.h"
#include "Plasma.h"
#include "Matrix.h"
#include "CrossHatch.h"
#include "Drops.h"
#include "Noise.h"
#include "Snake.h"

// Helper to map XY coordinates to irregular matrix
uint16_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  uint8_t i = (y * kMatrixWidth) + x;
  return i;
}

void setup() {
  FastLED.addLeds < CHIPSET, LED_PIN, COLOR_ORDER > (leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(brightness);
  FastLED.clear(true);

  modeBtn.begin();
  buttonPushCounter = 0;
  
  Serial.begin(57600);
  Serial.print(F("Starting pattern "));
  Serial.println(buttonPushCounter);
}



bool checkModeButton() {  
  modeBtn.read();

  switch (state) {
    case 0:                
      if (modeBtn.wasReleased()) {
        incrementButtonPushCounter();
        Serial.print(F("Short press, pattern "));
        Serial.println(buttonPushCounter);
        autoChangeVisuals = false;
        return true;
      }
      else if (modeBtn.pressedFor(LONG_PRESS)) {
        state = 1;
        return true;
      }
      break;

    case 1:
      if (modeBtn.wasReleased()) {
        state = 0;
        Serial.print(F("Long press, auto pattern "));
        // Go back to beginning
        buttonPushCounter = 0;
        autoChangeVisuals = true;
        return true;
      }
      break;
  }

  if(autoChangeVisuals){
    EVERY_N_SECONDS(PATTERN_TIME) {
      incrementButtonPushCounter();
      Serial.print("Auto, pattern ");
      Serial.println(buttonPushCounter); 
      return true;
    }
  }  

  return false;
}

// Functions to run patterns. Done this way so each class stays in scope only while
// it is active, freeing up RAM once it is changed.

void runRainbow(){
  bool isRunning = true;
  Rainbow rainbow = Rainbow();
  while(isRunning) isRunning = rainbow.runPattern();
}

void runFire(){
  bool isRunning = true;
  Fire fire = Fire();
  while(isRunning) isRunning = fire.runPattern();
}

void runSquares(){
  bool isRunning = true;
  Squares squares = Squares();
  while(isRunning) isRunning = squares.runPattern();
}

void runCircles(){
  bool isRunning = true;
  Circles circles = Circles();
  while(isRunning) isRunning = circles.runPattern();
}

void runPlasma(){
  bool isRunning = true;
  Plasma plasma = Plasma();
  while(isRunning) isRunning = plasma.runPattern();
}

void runMatrix(){
  bool isRunning = true;
  Matrix matrix = Matrix();
  while(isRunning) isRunning = matrix.runPattern();
}

void runCrossHatch(){
  bool isRunning = true;
  CrossHatch crossHatch = CrossHatch();
  while(isRunning) isRunning = crossHatch.runPattern();
}

void runDrops(){
  bool isRunning = true;
  Drops drops = Drops();
  while(isRunning) isRunning = drops.runPattern();
}

void runNoise(){
  bool isRunning = true;
  Noise noise = Noise();
  while(isRunning) {
    isRunning = noise.runPattern();
  }
}

void runSnake(){
  bool isRunning = true;
  Snake snake = Snake();
  while(isRunning) {
    isRunning = snake.runPattern();
  }
}

// Run selected pattern
void loop() {
  switch (buttonPushCounter) {
    case 0:
      runRainbow();
      break;
    case 1:
      runFire();
      break;
    case 2:
      runSquares();
      break;
    case 3:
      runCircles();
      break;
    case 4:
      runPlasma();
      break;
    case 5:
      runMatrix();
      break;
    case 6:
      runCrossHatch();
      break;
    case 7:
      runDrops();
      break;
    case 8:
      runNoise();
      break;
    case 9:
      runSnake();
      break;
  }
}
