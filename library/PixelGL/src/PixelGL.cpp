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
#include "PixelGL.h"
#include <Adafruit_NeoPixel.h>

RGBA8::RGBA8( void )
{
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
	
}

void RGBA8::clear( void )
{
	red = 0;
	green = 0;
	blue = 0;
	alpha = 0;
	
}

void RGBA8::copy( RGBA8 * inputColor )
{
	red = inputColor->red;
	green = inputColor->green;
	blue = inputColor->blue;
	alpha = inputColor->alpha;
	
}


//**********************************************************************//
//
//  Color Mixer
//
//**********************************************************************//
ColorMixer::ColorMixer(uint16_t n, uint8_t p, uint8_t t) : Adafruit_NeoPixel(n, p, t)
{
	stripWidth = n;
	mainPage = new RGBA8[stripWidth];

	for (int i = 0; i < stripWidth; i++)
	{
	mainPage[i].red = 0;
	mainPage[i].green = 0;
	mainPage[i].blue = 0;
	mainPage[i].alpha = 0;
	}
 
}

ColorMixer::~ColorMixer()
{
	delete[] mainPage;

}

void ColorMixer::clearPage( void )
{
  for (int i = 0; i < stripWidth; i++)
  {
    mainPage[i].red = 0;
    mainPage[i].green = 0;
    mainPage[i].blue = 0;
    mainPage[i].alpha = 0;
  }

}

void ColorMixer::addLayer( RGBA8 inputColor )
{
	//Temporary matrix
	RGBA8 outField[stripWidth];
	
	for( int i = 0; i < stripWidth; i++ )
	{
		outField[i] = inputColor;
	}
	
	//Call the mixer
	addLayer(outField);

}

void ColorMixer::addLayer( RGBA8 * inputColorMatrix )
{
	for (int i = 0; i < stripWidth; i++)
	{
		mainPage[i].red = (((uint16_t)inputColorMatrix[i].red * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].red * ( (uint16_t)256 - inputColorMatrix[i].alpha)) >> 8 );
		mainPage[i].green = (((uint16_t)inputColorMatrix[i].green * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].green * ( (uint16_t)256 - inputColorMatrix[i].alpha)) >> 8 );
		mainPage[i].blue = (((uint16_t)inputColorMatrix[i].blue * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].blue * ( (uint16_t)256 - inputColorMatrix[i].alpha)) >> 8 );
	
	}
	
}

void ColorMixer::orLayer( RGBA8 inputColor )
{
  for (int i = 0; i < stripWidth; i++)
  {
    if( inputColor.red > mainPage[i].red )
    {
      mainPage[i].red =  inputColor.red;
    }
    if( inputColor.green > mainPage[i].green )
    {
      mainPage[i].green =  inputColor.green;
    }
    if( inputColor.blue > mainPage[i].blue )
    {
      mainPage[i].blue =  inputColor.blue;
    }
  }

}

void ColorMixer::gradientAddLayer( RGBA8 point1Color, int16_t point1Position, RGBA8 point2Color, int16_t point2Position )
{
	//sanitize
	if( point1Position > ( stripWidth + 20 ) )
	{
		point1Position = ( stripWidth + 20 );
	}
	if( point1Position < -20 )
	{
		point1Position = -20;
	}
	if( point2Position > ( stripWidth + 20 ) )
	{
		point2Position = ( stripWidth + 20 );
	}
	if( point2Position < -20 )
	{
		point2Position = -20;
	}
	//Make sure position two is always greater than position 1
	if( point1Position > point2Position ) //then swap them
	{
		int16_t swapPosition = point2Position;
		point2Position = point1Position;
		point1Position = swapPosition;
		RGBA8 swapColor = point2Color;
		point2Color = point1Color;
		point1Color = swapColor;
	}
	
	//Temporary matrix
	RGBA8 outField[stripWidth];
	

	//Calc line for alpha
	int16_t slope = ((int16_t)point2Color.alpha - (int16_t)point1Color.alpha ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i < stripWidth ) )
		{
			outField[i].alpha = point1Color.alpha + ((i - point1Position) * slope);
		}
	}

	//Calc line for red
	slope = ((int16_t)point2Color.red - (int16_t)point1Color.red ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i < stripWidth ) )
		{
			outField[i].red = point1Color.red + ((i - point1Position) * slope);
		}
	}

	//Calc line for green
	slope = ((int16_t)point2Color.green - (int16_t)point1Color.green ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i < stripWidth ) )
		{
			outField[i].green = point1Color.green + ((i - point1Position) * slope);
		}
	}

	//Calc line for blue
	slope = ((int16_t)point2Color.blue - (int16_t)point1Color.blue ) / ( point2Position - point1Position ); //dHeight / dX
	for (int i = point1Position; i <= point2Position; i++)
	{
		if( ( i >= 0)&&( i < stripWidth ) )
		{
			outField[i].blue = point1Color.blue + ((i - point1Position) * slope);
		}
	}
	
	//Call the mixer
	addLayer(outField);
	
	
}

void ColorMixer::brush( int8_t position, RGBA8 * color, int8_t sideWall )
{
	//Draw two lines
	RGBA8 sidePixel;
	sidePixel = *color;
	sidePixel.alpha = 0;//sidePixel.alpha >> 1;
	
	gradientAddLayer( *color, position, sidePixel, position + sideWall );
	
	gradientAddLayer( *color, position, sidePixel, position - sideWall );
	
	
}

void ColorMixer::rotate( int16_t offset )
{
	//Temporary matrix
	RGBA8 outField[stripWidth];
	//Temp offset
	int16_t offsetVector = 0;
	
	for( int i = 0; i < stripWidth; i++ )
	{
		//roll over
		offsetVector = i + offset;
		while( offsetVector >= stripWidth )
		{
			offsetVector = offsetVector - stripWidth;
		}
		while( offsetVector < 0 )
		{
			offsetVector = offsetVector + stripWidth;
		}

		outField[i] = mainPage[offsetVector];
		outField[i].alpha = 255;
	}
	
	//Call the mixer
	addLayer(outField);
}

void ColorMixer::mix( void )
{
  for (int i = 0; i < stripWidth; i++)
  {
    setPixelColor(i, mainPage[i].red, mainPage[i].green, mainPage[i].blue);

  }

}
