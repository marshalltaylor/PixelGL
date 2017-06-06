//**********************************************************************//
//  BEERWARE LICENSE
//
//  This code is free for any use provided that if you meet the author
//  in person, you buy them a beer.
//
//  This license block is BeerWare itself.
//
//  Written by:  Marshall Taylor
//  Created:  March 21, 2015
//
//**********************************************************************//

//HOW TO OPERATE
//  Make TimerClass objects for each thing that needs periodic service
//  pass the interval of the period in ticks
//  Set maxInterval to the max foreseen interval of any TimerClass
//  Set maxTimer to overflow number in the header.  maxTimer + maxInterval
//    cannot exceed variable size.

//**General***********************************//
#include "stdint.h"

//**Timers and stuff**************************//
#include "Wire.h"
#include "timerModule.h"
#include <Adafruit_NeoPixel.h>
#include "PixelGL.h"
#include "colorMachines.h"

//If Teensy 3.1 based, do this
IntervalTimer myTimer;

TimerClass messageTimer( 10 );
TimerClass ledOutputTimer( 5 ); //For updating LEDs
TimerClass bootSequenceTimer( 1000 ); //For general output use
uint8_t bootSequenceCounter = 0;
TimerClass sparkleTimer( 2 ); //For general output use
TimerClass washOutTimer( 10 );
TimerClass debugTimer(80); //For general output use
uint16_t tempTimer = 0;

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

uint16_t maxTimer = 60000;
uint16_t maxInterval = 2000;


#define WS2812PIN 3
ColorMixer outputMixer(72, WS2812PIN, NEO_GRB + NEO_KHZ800);

//**Color state machines**********************//
int16_t bgOffset = 0;
FlashDialog messages;
float pointBrightness = 0;
int8_t pointPolarity = 1;
BrushPixel sparklePage[72];
uint8_t sparkleCounter = 0;
uint8_t sparkleCounterMax = 100;

WashOut redWash;
WashOut greenWash;
WashOut blueWash;

void setup()
{
	delay(2000);

	Serial.begin(115200);
	Serial.println("Program started\n");  
  
  //If 328p based, do this
#ifdef __AVR__
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B

  // set compare match register to desired timer count:
  OCR1A = 16000;

  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);

  // Set CS10 and CS12 bits for 1 prescaler:
  TCCR1B |= (1 << CS10);


  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);

  // enable global interrupts:
  sei();
#endif

//If Teensy 3.1 based, do this
#ifdef __MK20DX256__
  // initialize IntervalTimer
  myTimer.begin(serviceMS, 1000);  // serviceMS to run every 0.001 seconds
#endif
	
	outputMixer.begin();
	outputMixer.show();

	//messageColor.red = 128;

	//Make up the washes
	redWash.targetColor.red = 255;
	redWash.targetColor.alpha = 255;
	greenWash.targetColor.green = 255;
	greenWash.targetColor.alpha = 255;
	blueWash.targetColor.blue = 255;
	blueWash.targetColor.alpha = 255;

}

void loop()
{
	// main program

	if ( msTicksMutex == 0 ) //Only touch the timers if clear to do so.
	{
		//**Copy to make a new timer******************//
		//    msTimerA.update(msTicks);
		messageTimer.update(msTicks);
		ledOutputTimer.update(msTicks);
		bootSequenceTimer.update(msTicks);
		sparkleTimer.update(msTicks);
		washOutTimer.update(msTicks);
		debugTimer.update(msTicks);
		
		//Done?  Lock it back up
		msTicksMutex = 1;
	}  //The ISR should clear the mutex.

	//**Copy to make a new timer******************//
	//  if(msTimerA.flagStatus() == PENDING)
	//  {
	//    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
	//  }

	if (messageTimer.flagStatus() == PENDING)
	{
		messages.tick();
	}

	if (ledOutputTimer.flagStatus() == PENDING)
	{
		//Reset the field
		outputMixer.clearPage();
		//Start with a background if you want
		//outputMixer.addLayer((RGBA8*)background); //Background const array or somethin'

		//Build an image here
		//  Define color registers
		RGBA8 bgColor1;
		bgColor1.green = 255;
		bgColor1.blue = 255;
		bgColor1.red = 255;
		bgColor1.alpha = 10;
		Serial.println(bootSequenceCounter);

		RGBA8 point1;
		RGBA8 point2;
		RGBA8 point3;
		
		RGBA8 testField[8];
		testField[2].clear();
		testField[2].red = 100;
		testField[2].alpha = 255;
		
		//  Apply operations
		outputMixer.clearPage();
		outputMixer.addLayer(bgColor1);
		//outputMixer.addLayer(testField);

		testField[2].clear();
		testField[2].green = 100;
		testField[2].alpha = 255;
		//outputMixer.addLayer(testField);
		
		point1.green = 80;
		point1.blue = 0;
		point1.red = 0;
		point1.alpha = 120;
		
		point2.green = 0;
		point2.blue = 80;
		point2.red = 0;
		point2.alpha = 120;

		point3.green = 0;
		point3.blue = 0;
		point3.red = 80;
		point3.alpha = 120;
		
		if( bootSequenceCounter > 3 )
		{
			outputMixer.gradientAddLayer( point1, 0, point2, 23 );
			outputMixer.gradientAddLayer( point2, 24, point3, 47 );
			outputMixer.gradientAddLayer( point3, 48, point1, 71 );
		}
		
		if( bootSequenceCounter > 6 )
		{
			outputMixer.rotate( bgOffset );
		}
		else
		{
			//reset roller
			bgOffset = 0;
		}
		
		if(( bootSequenceCounter > 12 )&&( bootSequenceCounter < 21 )) //Blue blob
		{
			point1.green = 20;
			point1.blue = 120;
			point1.red = 20;
			point1.alpha = (uint8_t)(pointBrightness * 200);
			outputMixer.brush( 8, &point1, 4 );
			
			//bootSequenceCounter = 16; //Temp lock
		}
		
		if( bootSequenceCounter >= 21 )// Sparkle
		{
			RGBA8 tempColor;
			if( bootSequenceCounter > 40 )
			{
				//outputMixer.clearPage();
			}
			for( int i = 0; i < 71; i++ )
			{
				//outputMixer.brush( i, &sparklePage[i], sparklePage[i].sideWall );
			
			}
		}
		
		point1.red = ((uint16_t)redWash.outputColor.red * (uint16_t)redWash.outputColor.alpha) >> 8;
		point1.green = ((uint16_t)greenWash.outputColor.green * (uint16_t)greenWash.outputColor.alpha) >> 8;
		point1.blue = ((uint16_t)blueWash.outputColor.blue * (uint16_t)blueWash.outputColor.alpha) >> 8;
		
		//get biggest alpha
		uint8_t maxAlpha = 0;
		if( redWash.outputColor.alpha > maxAlpha ) maxAlpha = redWash.outputColor.alpha;
		if( greenWash.outputColor.alpha > maxAlpha ) maxAlpha = greenWash.outputColor.alpha;
		if( blueWash.outputColor.alpha > maxAlpha ) maxAlpha = blueWash.outputColor.alpha;
		point1.alpha = maxAlpha;
		
		outputMixer.addLayer( point1 );

		//Push the output
		outputMixer.mix();
		outputMixer.show();

	}
	if (bootSequenceTimer.flagStatus() == PENDING)
	{
		if( bootSequenceCounter < 100 )
		{
			bootSequenceCounter++;
		}
		else
		{
			//bootSequenceCounter = 0;
		}
	}
	if (sparkleTimer.flagStatus() == PENDING)
	{
		//Rando table
		// () wait up to 500ms... at 10ms a number 0 to 50
		// ()
		//
		//
		//Process the page
		for( int i = 0; i < 71; i++ )
		{
			//Work each pixel
			sparklePage[i].intensity = sparklePage[i].intensity + sparklePage[i].rate;
			if( sparklePage[i].intensity < 0 )
			{
				sparklePage[i].intensity = 0;
			}
			if( sparklePage[i].intensity > sparklePage[i].maxIntensity )
			{
				sparklePage[i].intensity = sparklePage[i].maxIntensity;
				sparklePage[i].rate = sparklePage[i].rate * -1;
			}
			sparklePage[i].alpha = sparklePage[i].intensity * 150;
		}
		if( sparkleCounter < sparkleCounterMax )
		{
			sparkleCounter++;
		}
		else
		{
			//set a new delay
			sparkleCounter = 0;
			sparkleCounterMax = 10 + random(10);
			
			//generate a new brush
			uint8_t tempLocation = random(71);
			sparklePage[tempLocation].red = 153;
			sparklePage[tempLocation].green = 48;
			sparklePage[tempLocation].blue = 191;
			sparklePage[tempLocation].sideWall = random(6);
			sparklePage[tempLocation].maxIntensity = .3 + .3 * (float)random(100)/100;
			float tempRate = 0.24 - 0.12 * ((float)random(100)/100);
			if( sparklePage[tempLocation].rate < tempRate )
			{
				if( sparklePage[tempLocation].intensity == 0 )
				{
					sparklePage[tempLocation].rate = tempRate;
				}
			}
			
		}
	
	}
	if (washOutTimer.flagStatus() == PENDING )
	{
		//Service all the washouts
		redWash.tick();
		greenWash.tick();
		blueWash.tick();
//		tempTimer++;
		if( tempTimer == 1000 )
		{
			greenWash.trigger();
		}
		if( tempTimer == 1200 )
		{
			redWash.trigger();
		}
		if( tempTimer == 1300 )
		{
			blueWash.trigger();
		}
		if( tempTimer > 1355 )
		{
			tempTimer = 0;
		}

	}
	if (debugTimer.flagStatus() == PENDING)
	{
		bgOffset = bgOffset + 1;
		if( bgOffset >= 72 )
		{
			bgOffset = 0;
		}
		pointBrightness = pointBrightness + ( pointPolarity *  0.05 );
		if( pointBrightness > 1 )
		{
			pointBrightness = 1;
			pointPolarity = -1;
		}
		if( pointBrightness < 0 )
		{
			pointBrightness = 0;
			pointPolarity = 1;
		}

		//Get all parameters
		//  Serial.print("\nPos:\n");
		//  Serial.print(" X = ");
		//  Serial.println(myIMU.scaledXX(), 4);
		//
		//  Serial.print("\nThermometer:\n");
		//  Serial.print(" Degrees C = ");
		//  Serial.println(myIMU.readTempC(), 4);
		//  Serial.print(" Degrees F = ");
		//  Serial.println(myIMU.readTempF(), 4);

		//Serial.println(mainSM.rightIn, 4);
		//Serial.println(testColor.red);

	}

}


//If 328p based, do this
#ifdef __AVR__
ISR(TIMER1_COMPA_vect)
#else
#endif
//If Teensy 3.1 based, do this
#ifdef __MK20DX256__
void serviceMS(void)
#else
#endif
{
  uint32_t returnVar = 0;
  if(msTicks >= ( maxTimer + maxInterval ))
  {
    returnVar = msTicks - maxTimer;

  }
  else
  {
    returnVar = msTicks + 1;
  }
  msTicks = returnVar;
  msTicksMutex = 0;  //unlock
}
