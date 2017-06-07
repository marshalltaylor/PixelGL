//********************************************//
#include "CustomPanel.h"
#include "panelComponents.h"
#include "HardwareInterfaces.h"
#include <Arduino.h>
#include "proto-8PanelComponents.h"

CustomPanel::CustomPanel( void )
{
	modeButton.setHardware(new ArduinoDigitalIn( 2 ), 1);
	add( &modeButton );

	dataKnob.setHardware(new ArduinoAnalogIn( A10 ));
	dataKnob.setWindow( 20 );
	dataKnob.setSamplesAveraged( 14 );
	add( &dataKnob );

	modeKnob.setHardware(new ArduinoAnalogIn( A10 ));
	modeKnob.init( 1023, 0, 10 );
	add( &modeKnob );

	state = PInit;
}

void CustomPanel::reset( void )
{
	//Set explicit states
	//Set all LED off
	state = PInit;
	
}

void CustomPanel::tickStateMachine( int msTicksDelta )
{
	freshenComponents( msTicksDelta );
	
	//***** PROCESS THE LOGIC *****//
	//Now do the states.
	PStates nextState = state;
	switch( state )
	{
	case PInit:
		nextState = PModeSelect;
		break;
	case PModeSelect:
		if( modeKnob.serviceChanged() )
		{
			Serial.print("mode: ");
			Serial.println(modeKnob.getState());
			mode = modeKnob.getState();
			switch( mode )
			{
			case 0:
				bgColor1.green = 255;
				bgColor1.blue = 255;
				bgColor1.red = 255;
				bgColor1.alpha = 10;

				featherColor1.green = 0;
				featherColor1.blue = 255;
				featherColor1.red = 110;
				featherColor1.alpha = 40;
				
				break;
			case 1:
				bgColor1.green = 0;
				bgColor1.blue = 255;
				bgColor1.red = 180;
				bgColor1.alpha = 40;

				featherColor1.green = 255;
				featherColor1.blue = 0;
				featherColor1.red = 80;
				featherColor1.alpha = 40;


				break;	
			case 2:
				color1.green = 80;
				color1.blue = 0;
				color1.red = 0;
				color1.alpha = 120;
				
				color2.green = 0;
				color2.blue = 80;
				color2.red = 0;
				color2.alpha = 120;
		
				color3.green = 0;
				color3.blue = 0;
				color3.red = 80;
				color3.alpha = 120;
				
				bgColor1.green = 255;
				bgColor1.blue = 255;
				bgColor1.red = 255;
				bgColor1.alpha = 10;
				break;		
			default:
				break;
			}

		}
		if( modeButton.serviceRisingEdge() )
		{
			nextState = PBackgroundBrightness;
		}
		break;		
	case PBackgroundBrightness:
		if( dataKnob.serviceChanged() )
		{
			Serial.print("backgroundBrightness: ");
			Serial.println(dataKnob.getState());
			backgroundBrightness = dataKnob.getState();
		}
		if( modeButton.serviceRisingEdge() )
		{
			nextState = PRainbowBrightness;
		}
		break;		
	case PRainbowBrightness:
		if( dataKnob.serviceChanged() )
		{
			Serial.print("rainbowBrightness: ");
			Serial.println(dataKnob.getState());
			rainbowBrightness = dataKnob.getState();
		}
		if( modeButton.serviceRisingEdge() )
		{
			nextState = PRainbowRate;
		}
		break;
	case PRainbowRate:
		if( dataKnob.serviceChanged() )
		{
			Serial.print("rainbowRate: ");
			Serial.println(dataKnob.getState());
			rainbowRate = dataKnob.getState();
		}
		if( modeButton.serviceRisingEdge() )
		{
			nextState = PModeSelect;
		}
		break;
	default:
		nextState = PInit;
		break;
	}
	state = nextState;

}