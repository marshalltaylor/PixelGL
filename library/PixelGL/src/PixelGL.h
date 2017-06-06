//**********************************************************************//
//  BEERWARE LICENSE
//
//  This code is free for any use provided that if you meet the author
//  in person, you buy them a beer.
//
//  This license block is BeerWare itself.
//
//  Written by:  Marshall Taylor
//  Created:  May 26, 2015
//
//**********************************************************************//
#ifndef PIXELGL_H_INCLUDED
#define PIXELGL_H_INCLUDED

#include "stdint.h"
#include <Adafruit_NeoPixel.h>

//Structures

//Classes

class RGBA8
{
public:
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
  
  RGBA8( void );
  void clear( void );
  void copy( RGBA8 * inputColor );
};

class BrushPixel : public RGBA8
{
public:
	float intensity;
	float maxIntensity;
	float rate;
	uint8_t sideWall;
	BrushPixel( void )
	{
		sideWall = 0;
		intensity = 0;
		maxIntensity = 0;
		rate = -0.05;
	}
	
};

class ColorMixer : public Adafruit_NeoPixel
{
public:
    ColorMixer(uint16_t n, uint8_t p=6, uint8_t t=NEO_GRB + NEO_KHZ800);// : Adafruit_NeoPixel(n, p, t);
	~ColorMixer( void );
    void clearPage( void );
    void addLayer( RGBA8 );//Color
    void addLayer( RGBA8 * );//Color array
    void orLayer( RGBA8 );//Color
	void gradientAddLayer( RGBA8, int16_t, RGBA8, int16_t );
	void brush( int8_t, RGBA8 *, int8_t );//position, color, side column width
	void rotate( int16_t );// shift up n
    void mix( void );

    RGBA8 * mainPage;
	
private:
  uint8_t stripWidth;

};


#endif // FILE_H_INCLUDED

