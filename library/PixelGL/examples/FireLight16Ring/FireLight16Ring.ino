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
#ifdef __MK20DX256__
IntervalTimer myTimer;
#endif



TimerClass ledOutputTimer( 5 ); //For updating LEDs
TimerClass transitTimer( 25 ); //For updating LEDs
TimerClass bootSequenceTimer( 30 ); //For general output use
uint8_t bootSequenceCounter = 0;
TimerClass debugTimer(1000); //For general output use
uint16_t tempTimer = 0;

uint16_t msTicks = 0;
uint8_t msTicksMutex = 1; //start locked out

uint16_t maxTimer = 60000;
uint16_t maxInterval = 2000;


#define WS2812PIN 3
ColorMixer outputMixer(16, WS2812PIN, NEO_GRB + NEO_KHZ400);


//  Common colors
RGBA8 bgColor1;
RGBA8 flame10;
RGBA8 flame15;
RGBA8 flame20;
RGBA8 flameU;
RGBA8 cVar1;
#include "timeKeeper.h"
TimeKeeper stateTimeKeeper;
TimeKeeper flickerTimeKeeper;

//**Color state machines**********************//
//int16_t bgOffset = 0;
//FlashDialog messages;
//float pointBrightness = 0;
//int8_t pointPolarity = 1;
//BrushPixel sparklePage[16];
//uint8_t sparkleCounter = 0;
//uint8_t sparkleCounterMax = 100;

//WashOut redWash;
//WashOut greenWash;
//WashOut blueWash;
Transit utilTransit;
Transit tTransit;
Transit bTransit;

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
//	redWash.targetColor.red = 255;
//	redWash.targetColor.alpha = 255;
//	greenWash.targetColor.green = 255;
//	greenWash.targetColor.alpha = 255;
//	blueWash.targetColor.blue = 255;
//	blueWash.targetColor.alpha = 255;
bgColor1.red = 0;
bgColor1.green = 0;
bgColor1.blue = 0;
bgColor1.alpha = 0;

utilTransit.setColor(&bgColor1, 20);
tTransit.setColor(&bgColor1, 20);
bTransit.setColor(&bgColor1, 20);

bgColor1.red = 30;
bgColor1.green = 30;
bgColor1.blue = 30;
bgColor1.alpha = 40;

flame10.red = 254;
flame10.green = 118;
flame10.blue = 29;
flame10.alpha = 120;

flame15.red = 254;
flame15.green = 118;
flame15.blue = 29;
flame15.alpha = 160;

flame20.red = 254;
flame20.green = 118;
flame20.blue = 29;
flame20.alpha = 200;

flameU.red = 254;
flameU.green = 118;
flameU.blue = 29;
flameU.alpha = 120;

}


uint8_t tState = 0;
uint16_t control = 100;
float holdControl = 1;
float dwellControl = 1;
float rateControl = 1;
int16_t lastKnobValue = 0;
uint8_t bTransitState = 0;

uint16_t tempAlpha;
uint16_t depth;
uint16_t rate1;
uint16_t rate2;

void loop()
{
	// main program

	if ( msTicksMutex == 0 ) //Only touch the timers if clear to do so.
	{
		//**Copy to make a new timer******************//
		//    msTimerA.update(msTicks);
		ledOutputTimer.update(msTicks);
		bootSequenceTimer.update(msTicks);
		debugTimer.update(msTicks);
		transitTimer.update(msTicks);
		
		//Done?  Lock it back up
		msTicksMutex = 1;
	}  //The ISR should clear the mutex.

	//**Copy to make a new timer******************//
	//  if(msTimerA.flagStatus() == PENDING)
	//  {
	//    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
	//  }
	if(transitTimer.flagStatus() == PENDING)
	{
		utilTransit.tick();
		tTransit.tick();
		bTransit.tick();
	}
	
	if (ledOutputTimer.flagStatus() == PENDING)
	{
		//Reset the field
		outputMixer.clearPage();
		//Start with a background if you want
		//outputMixer.addLayer((RGBA8*)background); //Background const array or somethin'

		//Build an image here
		if( lastKnobValue > 80 )
		{
			outputMixer.addLayer( utilTransit.outputColor );
		}
		//Serial.println(bootSequenceCounter);

		outputMixer.mix();
		outputMixer.show();
		stateTimeKeeper.mIncrement( 5 );
		flickerTimeKeeper.mIncrement( 5 );

	}
	if (bootSequenceTimer.flagStatus() == PENDING)
	{
		int16_t tempKnobValue = analogRead( A0 );
		if(( tempKnobValue > (lastKnobValue + 50) )||(tempKnobValue < (lastKnobValue - 50) ))
		{
			lastKnobValue = tempKnobValue;
//			Serial.println(lastKnobValue);
			flameU.alpha = lastKnobValue >> 2;
			tTransit.setColor(&flameU, 30);
			bTransitState = 0;
			bTransit.setColor(&flameU, 30);
			//tState = 4;
		}
		if( tTransit.busy() == 0)
		{
//			if( random(0, 10) > 4 )
//			{
//				//Transit
//				uint16_t tempAlpha = tTransit.outputColor.alpha;
//				uint16_t depth = random( -5, 5 );
//				uint16_t rate = random( 60, 255 );
//				tempAlpha += depth;
//				if( tempAlpha > 255 ) tempAlpha = 255;
//				if( tempAlpha < 0 ) tempAlpha = 0;
//				memcpy(&cVar1, &tTransit.outputColor, sizeof cVar1);
//				cVar1.alpha = tempAlpha;
//				tTransit.setColor( &cVar1, rate );
//				
//				tempAlpha -= 50;
//				if( tempAlpha < 0 ) tempAlpha = 0;
//				memcpy(&cVar1, &bTransit.outputColor, sizeof cVar1);
//				cVar1.alpha = tempAlpha;
//				bTransit.setColor( &cVar1, rate );
//				
//			}
		}
		if( bTransit.busy() == 0 )
		{
			switch( bTransitState )
			{
//				Serial.println("a");
				case 0: //Idle, random
//					Serial.println(" b");
					if( random(0, 100) < 10 )
					{
						//Transit
						if( random(0, 100) < 10 )
						{
							holdControl = ((float)random( 70, 100 )) / (float)100;
						}
						else
						{
							holdControl = ((float)random( 0, 40 )) / (float)100;
						}
						Serial.println(holdControl);
						dwellControl = holdControl;
//						Serial.println("boo");
						tempAlpha = tTransit.outputColor.alpha;
						depth = -30 - (holdControl * 100);
						rate1 = random( 30, 50 );
						tempAlpha += depth;
						if( tempAlpha > 255 ) tempAlpha = 255;
						if( tempAlpha < 0 ) tempAlpha = 0;
						memcpy(&cVar1, &bTransit.outputColor, sizeof cVar1);
						cVar1.alpha = tempAlpha;
						bTransit.setColor( &cVar1, rate1 );
						bTransitState = 1;
						flickerTimeKeeper.mClear();

					}
				break;
				case 1: //dwell
//					Serial.println(" b");
					if(flickerTimeKeeper.mGet() > ( 300 + (4000*dwellControl) ))
					{
						bTransitState = 2;
					}
				case 2: //ramp up
//					Serial.println(" b");
//					Serial.println("    yaa");
					tempAlpha = tTransit.outputColor.alpha;
					rate2 = random( 200, 255 );
					tempAlpha -= 15;
					if( tempAlpha < 0 ) tempAlpha = 0;
					memcpy(&cVar1, &tTransit.outputColor, sizeof cVar1);
					cVar1.alpha = tempAlpha;
					bTransit.setColor( &cVar1, rate2 );
					bTransitState = 3;
					flickerTimeKeeper.mClear();
				break;
				case 3: //hold until done
//					Serial.println(" b");
					if(flickerTimeKeeper.mGet() > 3000 + (1200*dwellControl) )
					{
						bTransitState = 0;
					}
				break;
				default:
				break;
			}
		}
		switch( tState )
		{
			case 0: //is rising
				if( utilTransit.done() )
				{
					utilTransit.setColor(&tTransit.outputColor, random( 5 + (3*holdControl), 8 + (3*holdControl) ) );
					tState = 1;
					//Serial.println("state");
				}
			break;
			case 1: //dwell
				if( utilTransit.done() )
				{
					stateTimeKeeper.mClear();
					tState = 2;
				}
			break;
			case 2:
				if(stateTimeKeeper.mGet() > ( 50 + (100*holdControl) ))
				{
					tState = 3;
				}
			break;
			case 3:
//				if( random(0, 10) < 3 )
//				{
//					utilTransit.setColor(&bTransit.outputColor, random( 10, 15 ));
//				}
//				else
//				{
					utilTransit.setColor(&bTransit.outputColor, 3 + (3*holdControl));
//				}
				tState = 0;
			break;
			case 4: //escape case
				if( utilTransit.done() )
				{
					tState = 0;
				}
			
			default:
			tState = 0;
		}
		
		//bootSequenceCounter++;
		//if( bootSequenceCounter == 10 )
		//{
		//	utilTransit.setColor( &flame10, 5 );
		//}
		//if( bootSequenceCounter >= 20 )
		//{
		//	utilTransit.setColor( &flame20, 5 );
		//	bootSequenceCounter = 0;
		//}

		
		
		
		//bootSequenceCounter++;
		//if( bootSequenceCounter < 2 )
		//{
		//	memcpy(&bgColor1, &flame10, sizeof bgColor1);
		//}
		//else if( bootSequenceCounter < 3 )
		//{
		//	memcpy(&bgColor1, &flame15, sizeof bgColor1);
		//}
		//else if( bootSequenceCounter < 4 )
		//{
		//	memcpy(&bgColor1, &flame10, sizeof bgColor1);
		//}
		//else if( bootSequenceCounter < 5 )
		//{
		//	memcpy(&bgColor1, &flame20, sizeof bgColor1);
		//	bootSequenceCounter = 0;
		//}

	}
	if (debugTimer.flagStatus() == PENDING)
	{

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
