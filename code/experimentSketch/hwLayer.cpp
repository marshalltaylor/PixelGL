#include "hwLayer.h"

void viewport::setPixelXY(uint32_t x, uint32_t y, int color)
{
	if( x % 2 )
	{
		//We're on a reverse col
		uint32_t num = 0;
		num = ( height * ( x + 1) ) - y - 1;
		setPixel( num, color );
	}
	else
	{
		//We're on straight grid
		uint32_t num = 0;
		num = y + ( height * x );
		setPixel( num, color );
	
	}
	
}

viewpage::viewpage( viewport* inputViewport )
{
	linkedViewport = inputViewport;
	xLowerLimit = -32;
	xUpperLimit =  32;
	yLowerLimit = -32;
	yUpperLimit =  32;
	width = xUpperLimit - xLowerLimit;
	height = yUpperLimit - yLowerLimit;
	uint32_t arraySize = width*height*3;
	viewpageMemory = new uint8_t[arraySize];   // Request memory for the variable
	for( int i = 0; i < arraySize; i++ )
	{
		viewpageMemory[i] = 0x00;
	}
	
}

void viewpage::setPixelXY(int32_t x, int32_t y, int color)
{
	//Data is organized as rows, offset width
	//offset x
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	viewpageMemory[ (tempy * width + tempx) * 3 + 2 ] = (color & 0x0000FF);
	viewpageMemory[ (tempy * width + tempx) * 3 + 1 ] = (color & 0x00FF00) >> 8;
	viewpageMemory[ (tempy * width + tempx) * 3 + 0 ] = (color & 0xFF0000) >> 16;
	
}

int64_t viewpage::getPixelXY(int32_t x, int32_t y)
{
	//Data is organized as rows, offset width
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	uint64_t tempColor = 0;
	tempColor = viewpageMemory[ (tempy * width + tempx) * 3 + 2 ];
	tempColor |= ((uint64_t)viewpageMemory[ (tempy * width + tempx) * 3 + 1 ]) << 8;
	tempColor |= ((uint64_t)viewpageMemory[ (tempy * width + tempx) * 3 + 0 ]) << 16;
	return tempColor;
	
}

void viewpage::show( void )
{
	//For now, hardcode quadrant 1
	for( int i = 0; i < 32; i++ )
	{
		for( int j = 0; j < 8; j++ )
		{
			linkedViewport->setPixelXY( i, (7 - j), getPixelXY(i, j));
		}
	}
	linkedViewport->setPixelXY(8,2,0x0F0000);
	linkedViewport->show();
}