/*  OctoWS2811 BasicTest.ino - Basic RGB LED Test
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.

  This test is useful for checking if your LED strips work, and which
  color config (WS2811_RGB, WS2811_GRB, etc) they require.
*/

#include <OctoWS2811.h>
#include "hwLayer.h"

const int ledsPerStrip = 256;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

viewport leds(ledsPerStrip, displayMemory, drawingMemory, config, 32, 8);

pGLObject myObject;


viewpage mainPage(&leds, &myObject);

#define RED    0x0F0000
#define GREEN  0x000F00
#define BLUE   0x00000F
#define YELLOW 0x0F0F00
#define PINK   0x0F0008
#define ORANGE 0x080800
#define WHITE  0x0F0F0F

float x1Delta = 1.2;
float y1Delta = 1.3;
float x2Delta = 1.1;
float y2Delta = 1.4;

float linex1 = 1;
float liney1 = 2;
float linex2 = 30;
float liney2 = 7;

Layer myLayer( 0,0,3,2 );

void setup() {
	myObject.setPoints( 1, 2, 30, 7 );
	Serial.begin(9600);
  leds.begin();
  mainPage.setPixelXY(10,0,RED);
  mainPage.setPixelXY(1,1,GREEN);
  mainPage.setPixelXY(2,2,BLUE);
  mainPage.setPixelXY(3,2,YELLOW);
  mainPage.setLayer( &myLayer, 0 );//Set myLayer as top
  mainPage.setLayerOffset( 0, 2, 2 );//Position layer 0 at +2, +2
  mainPage.show();
  delay(2000);
  Serial.println("Started");

}

void loop()
{
	linex1 += x1Delta;
	liney1 += y1Delta;
	linex2 += x2Delta;
	liney2 += y2Delta;

	if(( linex1 >= 31 )||( linex1 <= 0 ))
	{
		x1Delta *= -1;
	}
	if(( linex2 >= 31 )||( linex2 <= 0 ))
	{
		x2Delta *= -1;
	}
	if(( liney1 >= 7 )||( liney1 <= 0 ))
	{
		y1Delta *= -1;
	}
	if(( liney2 >= 7 )||( liney2 <= 0 ))
	{
		y2Delta *= -1;
	}
	delay(200);
	mainPage.clear();
	myObject.setPoints( linex1, liney1, linex2, liney2 );
  mainPage.setPixelXY(linex1, liney1,RED);
  mainPage.setPixelXY(linex2, liney2,GREEN);  
	mainPage.show();
}

