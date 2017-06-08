//Header
#ifndef CUSTOMPANEL_H_INCLUDED
#define CUSTOMPANEL_H_INCLUDED

#include "Panel.h"
#include "PanelComponents.h"
#include "HardwareInterfaces.h"
#include <Arduino.h>
#include "proto-8PanelComponents.h"
#include "PixelGL.h"

enum PStates
{
	PInit,
	PBackgroundBrightness,
	PRainbowBrightness,
	PRate1,
	PRate2,
	PSparkleMode
};

class CustomPanel : public Panel
{
public:
	CustomPanel( void );
	void reset( void );
	void tickStateMachine( int msTicksDelta );
	uint8_t getState( void ){
		return (uint8_t)state;
	}
	
	uint16_t backgroundBrightness = 1024;
	uint16_t rainbowBrightness = 1024;
	uint16_t rainbowRate = 31;
	uint16_t featherRate = 16;
	uint16_t mode = 0;
	uint16_t squeeze = 0;
	uint16_t sparkleMode = 1;
	
	RGBA8 bgColor1;
	RGBA8 featherColor1;
	RGBA8 color1;
	RGBA8 color2;
	RGBA8 color3;
	
private:
	//Internal Panel Components
	Button modeButton;
	Windowed10BitKnob dataKnob;
	Selector dataKnobSelector;
	Selector modeKnob;
	
	//State machine stuff  
	PStates state;

};

#endif