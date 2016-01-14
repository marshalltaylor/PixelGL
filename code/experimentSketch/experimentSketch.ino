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

//Not used by this sketch but dependant on one 
#include "Wire.h"

//Globals
uint32_t MAXTIMER = 60000000;
uint32_t MAXINTERVAL = 2000000;

#define LEDPIN 13
#include "timerModule32.h"
#include "stdint.h"

IntervalTimer myTimer; //Interrupt for Teensy

//**Copy to make a new timer******************//  
//TimerClass32 usTimerA( 20000 ); //20 ms
TimerClass32 debugTimer( 1000000 ); //20 ms
TimerClass32 dotTimer( 5000 );
TimerClass32 drawTimer( 100000 );

uint32_t usTicks = 0;

uint8_t usTicksLocked = 1; //start locked out

uint8_t debugBlink = 0;







const int ledsPerStrip = 256;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

viewport leds(ledsPerStrip, displayMemory, drawingMemory, config, 32, 8);



viewpage mainPage(&leds);

#define RED    0x0F0000
#define GREEN  0x000F00
#define BLUE   0x00000F
#define YELLOW 0x0F0F00
#define PINK   0x0F0008
#define ORANGE 0x080800
#define WHITE  0x0F0F0F

float xBallDelta[15] = {0.012,0.023,0.015,0.032,0.008,0.054,0.023,0.028,0.024,0.018,0.032,0.011,0.012,0.013,0.014};
float yBallDelta[15] = {0.013,0.020,0.008,0.054,0.023,0.028,0.024,0.018,0.032,0.011,0.012,0.013,0.014,0.032,0.008};

float xBall[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float yBall[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

float x1Delta = .02;
float y1Delta = .02;
float dotx1 = 1;
float doty1 = 2;

float x2Delta = .03;
float y2Delta = .04;
float dotx2 = 1;
float doty2 = 2;


Layer myLayer( 0,0,3,2 );
Layer dotLayer( 0,0,5,5 );
Layer lineLayer( -2,-2,34,10 );
Layer fullLayer( -2,-2,34,10 );

PaintTools draw;

void setup() {
	delay(2000);
	Serial.begin(9600);
	Serial.println("Started after 2 second delay.");
	Serial.print("Size of RGBA8: ");
	Serial.println(PIXELMEMSIZE);

	myTimer.begin(serviceUS, 1);  // serviceMS to run every 0.000001 seconds
	//myObject.setPoints( 1, 2, 30, 7 );
	leds.begin();
	//mainPage.setPixelXY(10,0,RED);
	//mainPage.setPixelXY(1,1,GREEN);
	//mainPage.setPixelXY(2,2,BLUE);
	//mainPage.setPixelXY(3,2,YELLOW);
	//draw.dot( &dotLayer, 2.3, 2.2, RED );
	mainPage.setLayer( &myLayer, 0 );//Set myLayer as top
	mainPage.setLayer( &dotLayer, 1 );//
	mainPage.setLayer( &lineLayer, 2 );//
	mainPage.setLayer( &fullLayer, 3 );//
	mainPage.setLayerOffset( 0, 2, 0 );//Position layer 0 at +2, +2
	mainPage.setLayerOffset( 1, 6, 1 );//Position layer 0 at +2, +2
	mainPage.setLayerOffset( 2, 0, 0 );//Position layer 0 at +2, +2
	mainPage.setLayerOffset( 3, 0, 0 );
	
	myLayer.clear();
	dotLayer.clear();
	mainPage.show();
	Serial.println("Setup loop completing.");
		RGBA8 tempColor;
		tempColor.red = 100;
		tempColor.green = 0;
		tempColor.blue = 0;
		tempColor.alpha = 255;
		
		draw.dot( &fullLayer, 12.5, 3.1, &tempColor );
		mainPage.show();
		//while(1);

}

void loop()
{
	RGBA8 tempColor;
	//linex1 += x1Delta;
	//liney1 += y1Delta;
	//linex2 += x2Delta;
	//liney2 += y2Delta;
    //
	//if(( linex1 >= 31 )||( linex1 <= 0 ))
	//{
	//	x1Delta *= -1;
	//}
	//if(( linex2 >= 31 )||( linex2 <= 0 ))
	//{
	//	x2Delta *= -1;
	//}
	//if(( liney1 >= 7 )||( liney1 <= 0 ))
	//{
	//	y1Delta *= -1;
	//}
	//if(( liney2 >= 7 )||( liney2 <= 0 ))
	//{
	//	y2Delta *= -1;
	//}
	//delay(200);
	//mainPage.clear();
	//myObject.setPoints( linex1, liney1, linex2, liney2 );
	//mainPage.setPixelXY(linex1, liney1,RED);
	//mainPage.setPixelXY(linex2, liney2,GREEN);  
	//mainPage.show();
	
		//Update the timers, but only once per interrupt
	if( usTicksLocked == 0 )
	{
		//**Copy to make a new timer******************//  
		//msTimerA.update(usTicks);
		
		debugTimer.update(usTicks);
		dotTimer.update(usTicks);
		drawTimer.update(usTicks);
		//Done?  Lock it back up
		usTicksLocked = 1;
	}  //The ISR will unlock.

	
	if(debugTimer.flagStatus() == PENDING)
	{
		//User code
		debugBlink ^= 0x01;
		Serial.print("Free ram:");
		Serial.println(FreeRam());
	}
	if(dotTimer.flagStatus() == PENDING)
	{
		//User code
		dotx1 += x1Delta;
		doty1 += y1Delta;
	
		if(( dotx1 >= 31 )||( dotx1 <= 0 ))
		{
			x1Delta *= -1;
		}
		if(( doty1 >= 15 )||( doty1 <= -4 ))
		{
			y1Delta *= -1;
		}
		
		dotx2 += x2Delta;
		doty2 += y2Delta;
	
		if(( dotx2 >= 31 )||( dotx2 <= 0 ))
		{
			x2Delta *= -1;
		}
		if(( doty2 >= 15 )||( doty2 <= -4 ))
		{
			y2Delta *= -1;
		}
		
		for( int i = 0; i < 15; i++ )
		{
			xBall[i] += xBallDelta[i];
			yBall[i] += yBallDelta[i];
			if(( xBall[i] >= 31 )||( xBall[i] < 0 ))
			{
				xBallDelta[i] *= -1;
			}
			yBall[i] += yBallDelta[i];
			if(( yBall[i] >= 15 )||( yBall[i] < -4 ))
			{
				yBallDelta[i] *= -1;
			}			
		}		
		
	}
	if(drawTimer.flagStatus() == PENDING)
	{
		//User code
		mainPage.clear();
		lineLayer.clear();
		fullLayer.clear();

		//Balls
		tempColor.red = 12;
		tempColor.green = 5;
		tempColor.blue = 10;
		tempColor.alpha = 255;
		for( int i = 0; i < 15; i++ )
		{
			draw.dot( &fullLayer, xBall[i], yBall[i], &tempColor );
		}
		
		//Line
		tempColor.red = 12;
		tempColor.green = 12;
		tempColor.blue = 12;
		tempColor.alpha = 255;
		draw.line( &lineLayer, dotx1, doty1, dotx2, doty2, &tempColor );
		
		tempColor.red = 32;
		tempColor.green = 0;
		tempColor.blue = 0;
		tempColor.alpha = 255;
		draw.dot( &fullLayer, dotx1, doty1, &tempColor );
		
		tempColor.red = 0;
		tempColor.green = 32;
		tempColor.blue = 0;
		tempColor.alpha = 255;
		draw.dot( &fullLayer, dotx2, doty2, &tempColor );
		
		//debug
		if( debugBlink )
		{
			tempColor.red = 20;
			tempColor.green = 0;
			tempColor.blue = 0;
			tempColor.alpha = 255;
		}
		else
		{
			tempColor.red = 0;
			tempColor.green = 0;
			tempColor.blue = 0;
			tempColor.alpha = 255;
		}
		fullLayer.setPixelXY(0,0, &tempColor);
		
		mainPage.show();
	}
	
}


void serviceUS(void)
{
  uint32_t returnVar = 0;
  if(usTicks >= ( MAXTIMER + MAXINTERVAL ))
  {
    returnVar = usTicks - MAXTIMER;

  }
  else
  {
    returnVar = usTicks + 1;
  }
  usTicks = returnVar;
  usTicksLocked = 0;  //unlock
}

#ifdef __arm__
    // should use uinstd.h to define sbrk but Due causes a conflict
    extern "C" char* sbrk(int incr);
#else  // __ARM__
    extern char *__brkval;
    extern char __bss_end;
#endif  // __arm__

// function from the sdFat library (SdFatUtil.cpp)
// licensed under GPL v3
// Full credit goes to William Greiman.
int FreeRam() {
    char top;
    #ifdef __arm__
        return &top - reinterpret_cast<char*>(sbrk(0));
    #else  // __arm__
        return __brkval ? &top - __brkval : &top - &__bss_end;
    #endif  // __arm__
}