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
	PModeSelect,
	PBackgroundBrightness,
	PRainbowBrightness,
	PRainbowRate
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
	uint16_t mode = 0;

	RGBA8 bgColor1;
	RGBA8 color1;
	RGBA8 color2;
	RGBA8 color3;
	
private:
	//Internal Panel Components
	Button modeButton;
	Windowed10BitKnob dataKnob;
	Selector modeKnob;
	
	//State machine stuff  
	PStates state;

};

#endif