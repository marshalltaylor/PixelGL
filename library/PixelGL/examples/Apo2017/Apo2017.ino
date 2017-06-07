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
TimerClass32 sparkleTimer( 2000 ); //For general output use
TimerClass32 washOutTimer( 10000 );
TimerClass32 rainbowRotateTimer( 30000 );

uint8_t usTicksLocked = 1; //start locked out

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
ColorMixer featherMixer(30, FEATHERPIN, NEO_GRB + NEO_KHZ800);
float rainbowRotateFloat = 0;
float featherRotateFloat = 0;

void setup()
{
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
			featherRotateFloat = featherRotateFloat + 30 * (float)hatPanel.rainbowRate / 1023;
			while( featherRotateFloat >= 30 )
			{
				featherRotateFloat = featherRotateFloat - 30;
			}
		}
	}
	if(ledOutputTimer.flagStatus() == PENDING)
	{
		stateTimeKeeper.uIncrement(INPUT_TICK_US);

		MStates nextState = state;
		switch( hatPanel.mode )
		{
		case 0:
			blackMask.alpha = (255 * (int32_t)(1023 - hatPanel.backgroundBrightness)) / 1023;
			outputMixer.clearPage();
			outputMixer.addLayer(hatPanel.bgColor1);
			outputMixer.addLayer(blackMask);

			featherMixer.clearPage();
			featherMixer.addLayer(hatPanel.featherColor1);
			featherMixer.addLayer(blackMask);
			break;
		case 1:
		blackMask.alpha = (255 * (int32_t)(1023 - hatPanel.backgroundBrightness)) / 1023;
			outputMixer.clearPage();
			outputMixer.addLayer(hatPanel.bgColor1);
			outputMixer.addLayer(blackMask);

			featherMixer.clearPage();
			featherMixer.addLayer(hatPanel.featherColor1);
			featherMixer.addLayer(blackMask);
			break;	
		case 2:
			point1.copy( &hatPanel.color1 );
			point1.alpha = (point1.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point2.copy( &hatPanel.color2 );
			point2.alpha = (point2.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point3.copy( &hatPanel.color3 );
			point3.alpha = (point3.alpha * (uint16_t)hatPanel.rainbowBrightness) / 1023;
			point4.copy( &hatPanel.bgColor1 );
			point4.alpha = (point4.alpha * (uint16_t)hatPanel.backgroundBrightness) / 1023;
           
			outputMixer.clearPage();
			outputMixer.addLayer(point4);
			
			outputMixer.gradientAddLayer( point1, 0, point2, 23 );
			outputMixer.gradientAddLayer( point2, 24, point3, 47 );
			outputMixer.gradientAddLayer( point3, 48, point1, 71 );
			
			outputMixer.rotate( rainbowRotateFloat );
			break;		

		default:
			break;
		}
		//Push the output
		outputMixer.mix();
		outputMixer.show();
		featherMixer.mix();
		featherMixer.show();
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

