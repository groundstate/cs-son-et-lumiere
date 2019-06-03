
//
// leddriver.ino Drives LEDs in the Cs beam tube son et lumiere 
//

// The MIT License (MIT)
//
// Copyright (c)  2019  Michael J. Wouters
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <Adafruit_NeoPixel.h>

#define  PIN 6
#define  NPIXELS 18
#define  NSEQ  64

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NPIXELS, PIN, NEO_RGB + NEO_KHZ800);

uint8_t seqcnt=0;
uint8_t pixstate[NPIXELS];
uint8_t randseq[NSEQ];

#define BSEL1 3 // pixel position of first B-field state selector
#define BSTOP1  7
#define PULSE1 14

int cnt = 0;
uint32_t start=0;

void setup() {
  // Generate a sequence of random states that is cycled through
  for (uint8_t n=0;n<NSEQ;n++)
    randseq[n] = random(0,3);
  for (uint8_t n=0;n<NPIXELS;n++)
    pixstate[n]=0; // all off
  strip.begin();
  strip.show(); // initialize all pixels to 'off'
  Serial.begin(9600);
  start=millis();
}

void loop() {
  
  
  // If you work backwards you don't need no stinkin' temporaries
  
  // SEGMENT 4: atoms in superposition after first Ramsey pulse
  for (uint8_t n=17;n>PULSE1;n--){
    pixstate[n]=pixstate[n-1];
  }
  if (pixstate[PULSE1-1]>0)
    pixstate[PULSE1] = 3;
  else
    pixstate[PULSE1] = 0;
  
  // SEGMENT 3: atoms in lower state moving between selector 1 and first Ramsey pulse
  for (uint8_t n=PULSE1 - 1 ;n > BSTOP1+1;n--){
    pixstate[n]=pixstate[n-1];
  }
  if (pixstate[BSEL1]==1)
    pixstate[BSTOP1+1] = 1;
  else
    pixstate[BSTOP1+1] = 0;
  
  
  // SEGMENT 2: deflected atoms in upper state
  for (uint8_t n=BSTOP1 ;n > BSEL1 + 1;n--){
    pixstate[n]=pixstate[n-1];
  }
  if (pixstate[BSEL1]==2)
    pixstate[BSEL1+1] = 2;
  else
    pixstate[BSEL1+1] = 0;
    
  // SEGMENT 1
  for (uint8_t n=BSEL1 ;n >=1;n--){
    pixstate[n]=pixstate[n-1];
  }
  
  // Inject a new particle with 'random' state
  pixstate[0] = randseq[seqcnt];

  // Set all pixel colours
  for (uint8_t n=0;n<NPIXELS;n++){
    if (pixstate[n]==0)
      strip.setPixelColor(n, strip.Color(0, 0, 0));
    else if (pixstate[n]==1)
      strip.setPixelColor(n, strip.Color(16, 0, 0));
    else if (pixstate[n]==2)
      strip.setPixelColor(n, strip.Color(0, 0, 16));
    else if (pixstate[n]==3)
      strip.setPixelColor(n, strip.Color(16, 0, 16));
  }
 
  seqcnt++;
  if (seqcnt == NSEQ) seqcnt = 0;
  strip.show();
  cnt++;
  if (cnt==100){ // 75 ms for 100 iterations
    //Serial.print(millis()-start);
    start=millis();
    cnt=0;
  }
  delay(300);
  
}

 
  







