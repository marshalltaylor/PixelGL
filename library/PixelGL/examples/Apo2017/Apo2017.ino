// This is a template for using the uCModules panel stuff to make a program.
// The example makes some components and basically does nothing.
// To make your own,
// 
// 1. Copy all three files into a new project folder
// 2. Rename the .ino to match the folder
// 3. Rename CustomPanel.cpp and .h to something regarding your project
//     (ex: DoorbellPanel)
//     Also change #include
// 4. Modify the custom panel:
//   a. Rename guards
//   b. Change class name in both files
// 5. Change the type of myCustomPanel to your new name
//     (Give panel useful name here too)
// 
// Now the code should compile.  Go on,
// 
// 6. Add components within the new panel's class (DoorbellPanel.h)
// 7. Initialize these components inside the new panel (DoorbellPanel::DoorbellPanel())
// 8. Add custom variables to the panel and initialize inside constructor
// 9. List desired states in header file (PStates)
// 10. Expand state machine with new state logic.

#include "stdint.h"
#include "CustomPanel.h" //Edit and rename this
#include "timeKeeper32.h"
#include "timerModule32.h"
#include <Adafruit_NeoPixel.h>
#include "PixelGL.h"
#include "colorMachines.h"

#define LEDPIN 13

IntervalTimer ISRTimer; //Interrupt for Teensy

//**timer******************//  
#define INPUT_TICK_US 5000
uint32_t maxTimer = 60000000;
uint32_t maxInterval = 2000000;
uint32_t usTicks = 0;

TimerClass32 inputTickTimer( INPUT_TICK_US );
TimerClass32 debugTimer( 1000000 );
TimerClass32 ledOutputTimer( 5000 ); //For updating LEDs
//TimerClass32 sparkleTimer( 2000 ); //For general output use
TimerClass32 washOutTimer( 3000 );
TimerClass32 rainbowRotateTimer( 30000 );

uint8_t usTicksLocked = 1; //start locked out

FlashDialog messages;
float pointBrightness = 0;
BrushPixel sparklePage[50];
uint8_t sparkleCounter = 0;
uint8_t sparkleCounterMax = 100;

WashOut redWash;
WashOut greenWash;
WashOut blueWash;


enum MStates
{
	MInit,
	MBoot,
	MFadeIn,
	MRainbow
};

MStates state;
TimeKeeper32 stateTimeKeeper;

CustomPanel hatPanel;

#define WS2812PIN 3
ColorMixer outputMixer(72, WS2812PIN, NEO_GRB + NEO_KHZ800);
#define FEATHERPIN 6
ColorMixer featherMixer(14, FEATHERPIN, NEO_GRB + NEO_KHZ800);
float rainbowRotateFloat = 0;
float featherRotateFloat = 0;

void setup()
{
	//Make up the washes
	redWash.targetColor.red = 255;
	redWash.targetColor.alpha = 200;
	redWash.aRate = 200;
	redWash.rRate = 600;
	greenWash.targetColor.green = 255;
	greenWash.targetColor.alpha = 100;
	greenWash.aRate = 200;
	greenWash.rRate = 400;
	blueWash.targetColor.blue = 255;
	blueWash.targetColor.alpha = 100;

	
	state = MInit;

	Serial.begin(115200);
	delay(1000);
	Serial.println("Started");

	pinMode( 2, INPUT_PULLUP );
	//Go to fresh state
	hatPanel.reset();

	// initialize IntervalTimer
	ISRTimer.begin(serviceUS, 1);  // serviceMS to run every 0.000001 seconds

	outputMixer.begin();
	outputMixer.show();			

	featherMixer.begin();
	featherMixer.show();			

}
RGBA8 blackMask;
RGBA8 point1;
RGBA8 point2;
RGBA8 point3;
RGBA8 point4;

void loop()
{
	if( usTicksLocked == 0 )
	{
		inputTickTimer.update(usTicks);
		debugTimer.update(usTicks);
		ledOutputTimer.update(usTicks);
		rainbowRotateTimer.update(usTicks);
		//sparkleTimer.update(usTicks);
		washOutTimer.update(usTicks);
		
		usTicksLocked = 1;
	}  //The ISR will unlock.


	if(debugTimer.flagStatus() == PENDING)
	{
		digitalWrite( LEDPIN, digitalRead( LEDPIN ) ^ 0x01 );
		Serial.println("tick");
	}

	if(inputTickTimer.flagStatus() == PENDING)
	{
		hatPanel.tickStateMachine(INPUT_TICK_US);
	}
	if(rainbowRotateTimer.flagStatus() == PENDING)
	{
		if( hatPanel.rainbowRate > 10 )
		{
			rainbowRotateFloat = rainbowRotateFloat + 30 * (float)hatPanel.rainbowRate / 1023;
			while( rainbowRotateFloat >= 72 )
			{
				rainbowRotateFloat = rainbowRotateFloat - 72;
			}
		}
		if( hatPanel.rainbowRate > 10 )
		{
			featherRotateFloat = featherRotateFloat - 15 * (float)hatPanel.featherRate / 1023;
			while( featherRotateFloat < 0 )
			{
				featherRotateFloat = featherRotateFloat + 30;
			}
		}
	}
	if(ledOutputTimer.flagStatus() == PENDING)
	{
		outputMixer.clearPage();
		featherMixer.clearPage();

		int temp = hatPanel.mode;
		if( (temp==0) || (temp==1) )
		{
			blackMask.alpha = (255 * (int32_t)(1023 - hatPanel.backgroundBrightness)) / 1023;
			outputMixer.addLayer(hatPanel.bgColor1);
			outputMixer.addLayer(blackMask);

			featherMixer.addLayer(hatPanel.featherColor1);
			featherMixer.addLayer(blackMask);
		}
		if( (temp==2) || (temp==3) )
		{
			point1.copy( &hatPanel.color1 );
			point1.alpha = (point1.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point2.copy( &hatPanel.color2 );
			point2.alpha = (point2.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point3.copy( &hatPanel.color3 );
			point3.alpha = (point3.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point4.copy( &hatPanel.bgColor1 );
			point4.alpha = (point4.alpha * (uint16_t)hatPanel.backgroundBrightness) / 1023;
           
			outputMixer.addLayer(point4);
			switch( hatPanel.squeeze )
			{
				case 0:
					outputMixer.gradientAddLayer( point1, 0, point2, 23 );
					outputMixer.gradientAddLayer( point2, 24, point3, 47 );
					outputMixer.gradientAddLayer( point3, 48, point1, 71 );
				default:
				break;
				case 1:
					outputMixer.gradientAddLayer( point1, 0, point2, 11 );
					outputMixer.gradientAddLayer( point2, 12, point3, 23 );
					outputMixer.gradientAddLayer( point3, 24, point1, 35 );
					outputMixer.gradientAddLayer( point1, 36, point2, 47 );
					outputMixer.gradientAddLayer( point2, 48, point3, 59 );
					outputMixer.gradientAddLayer( point3, 60, point1, 71 );
				break;
				case 2:
					outputMixer.gradientAddLayer( point1, 0, point2, 5 );
					outputMixer.gradientAddLayer( point2, 6, point3, 11 );
					outputMixer.gradientAddLayer( point3, 12, point1, 17 );
					outputMixer.gradientAddLayer( point1, 18, point2, 23 );
					outputMixer.gradientAddLayer( point2, 24, point3, 29 );
					outputMixer.gradientAddLayer( point3, 30, point1, 35 );
					outputMixer.gradientAddLayer( point1, 36, point2, 41 );
					outputMixer.gradientAddLayer( point2, 42, point3, 47 );
					outputMixer.gradientAddLayer( point3, 48, point1, 53 );
					outputMixer.gradientAddLayer( point1, 54, point2, 59 );
					outputMixer.gradientAddLayer( point2, 60, point3, 65 );
					outputMixer.gradientAddLayer( point3, 66, point1, 71 );

				break;
			}
			outputMixer.rotate( rainbowRotateFloat );

			point1.copy( &hatPanel.featherColor1 );
			point1.alpha = (point1.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point2.copy( &hatPanel.featherColor2 );
			point2.alpha = (point2.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			featherMixer.gradientAddLayer( point1, 0, point2, 6 );
			featherMixer.gradientAddLayer( point2, 7, point3, 14 );
			featherMixer.rotate( featherRotateFloat );

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
		featherMixer.mix();
		featherMixer.show();
	}
	if (washOutTimer.flagStatus() == PENDING )
	{
		//Service all the washouts
		redWash.tick();
		greenWash.tick();
		blueWash.tick();


	}
	
}

void serviceUS(void)
{
  uint32_t returnVar = 0;
  if(usTicks >= ( maxTimer + maxInterval ))
  {
    returnVar = usTicks - maxTimer;

  }
  else
  {
    returnVar = usTicks + 1;
  }
  usTicks = returnVar;
  usTicksLocked = 0;  //unlock
}

