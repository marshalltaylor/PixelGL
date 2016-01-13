#include "hwLayer.h"
#include <math.h>

//**********************************************************************//
//
//  Color stuff
//
//**********************************************************************//

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

//**********************************************************************//
//
//  Viewport
//
//**********************************************************************//

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

//**********************************************************************//
//
//  Layer
//
//**********************************************************************//
Layer::Layer( int32_t xLVar, int32_t yLVar, int32_t xUVar, int32_t yUVar)
{
	xOffset = 0;
	yOffset = 0;
	xLowerLimit = xLVar;
	xUpperLimit = xUVar;
	yLowerLimit = yLVar;
	yUpperLimit = yUVar;
	width = xUpperLimit - xLowerLimit + 1;
	height = yUpperLimit - yLowerLimit + 1;
	uint32_t arraySize = width*height*3;
	layerMemory = new uint8_t[arraySize];   // Allocate memory for the variable
	for( int i = 0; i < arraySize; i++ )
	{
		layerMemory[i] = 0;
	}
	
}

Layer::Layer( int32_t xLVar, int32_t yLVar, int32_t xUVar, int32_t yUVar, uint8_t debugFlag)
{
	xOffset = 0;
	yOffset = 0;
	xLowerLimit = xLVar;
	xUpperLimit = xUVar;
	yLowerLimit = yLVar;
	yUpperLimit = yUVar;
	width = xUpperLimit - xLowerLimit + 1;
	height = yUpperLimit - yLowerLimit + 1;
	uint32_t arraySize = width*height*3;
	layerMemory = new uint8_t[arraySize];   // Allocate memory for the variable
	for( int i = 0; i < arraySize; i++ )
	{
		if(debugFlag)
		{
			layerMemory[i] = ((float)i / (float)arraySize * 7) + 1;
		}
		else
		{
			layerMemory[i] = 0;
		}
	}
	
}

void Layer::clear( void )
{
	uint32_t arraySize = width*height*3;
	for( int i = 0; i < arraySize; i++ )
	{
		layerMemory[i] = 0;
	}
	
}

void Layer::setPixelXY(int32_t x, int32_t y, int color)
{
	//Data is organized as rows, offset width
	//offset x
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	layerMemory[ (tempy * width + tempx) * 3 + 2 ] = (color & 0x0000FF);
	layerMemory[ (tempy * width + tempx) * 3 + 1 ] = (color & 0x00FF00) >> 8;
	layerMemory[ (tempy * width + tempx) * 3 + 0 ] = (color & 0xFF0000) >> 16;
	
}

int64_t Layer::getPixelXY(int32_t x, int32_t y)
{
	//Data is organized as rows, offset width
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	uint64_t tempColor = 0;
	tempColor = layerMemory[ (tempy * width + tempx) * 3 + 2 ];
	tempColor |= ((uint64_t)layerMemory[ (tempy * width + tempx) * 3 + 1 ]) << 8;
	tempColor |= ((uint64_t)layerMemory[ (tempy * width + tempx) * 3 + 0 ]) << 16;
	return tempColor;
	
}

void Layer::process( void )
{

}


//**********************************************************************//
//
//  pGLObject -- might be defunct
//
//**********************************************************************//

pGLObject::pGLObject( void )
{
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;
	
}

void pGLObject::setPoints( int32_t x1Var, int32_t y1Var, int32_t x2Var, int32_t y2Var )
{
	x1 = x1Var;
	y1 = y1Var;
	x2 = x2Var;
	y2 = y2Var;
	
}



//**********************************************************************//
//
//  Viewpage
//
//**********************************************************************//


viewpage::viewpage( viewport* inputViewport, pGLObject* inputObject )
{
	linkedViewport = inputViewport;
	linkedObjectHead = inputObject;
	xLowerLimit = -32;
	xUpperLimit =  32;
	yLowerLimit = -32;
	yUpperLimit =  32;
	width = xUpperLimit - xLowerLimit + 1;
	height = yUpperLimit - yLowerLimit + 1;
	uint32_t arraySize = width*height*3;
	viewpageMemory = new uint8_t[arraySize];   // Allocate memory for the variable
	for( int i = 0; i < arraySize; i++ )
	{
		viewpageMemory[i] = 0x00;
	}
	
}

void viewpage::clear( void )
{
	uint32_t arraySize = width*height*3;
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
	//Process top layer
	layers[0]->process(); //Does nothing
	//Now copy the layer memory over
	for( int i = layers[0]->xLowerLimit; i <= layers[0]->xUpperLimit; i++ )
	{
		for( int j = layers[0]->yLowerLimit; j <= layers[0]->yUpperLimit; j++ )
		{
			setPixelXY( i + layers[0]->xOffset, j + layers[0]->yOffset, layers[0]->getPixelXY(i, j));
		}
	}
	//Now copy the layer memory over
	for( int i = layers[1]->xLowerLimit; i <= layers[1]->xUpperLimit; i++ )
	{
		for( int j = layers[1]->yLowerLimit; j <= layers[1]->yUpperLimit; j++ )
		{
			setPixelXY( i + layers[1]->xOffset, j + layers[1]->yOffset, layers[1]->getPixelXY(i, j));
		}
	}
	//Now copy the layer memory over
	for( int i = layers[2]->xLowerLimit; i <= layers[2]->xUpperLimit; i++ )
	{
		for( int j = layers[2]->yLowerLimit; j <= layers[2]->yUpperLimit; j++ )
		{
			setPixelXY( i + layers[2]->xOffset, j + layers[2]->yOffset, layers[2]->getPixelXY(i, j));
		}
	}	
	
	//Process our 1 temp object, pGLObject
	//Step through x, find y
	int32_t tempx1 = linkedObjectHead->x1;
	int32_t tempy1 = linkedObjectHead->y1;
	int32_t tempx2 = linkedObjectHead->x2;
	int32_t tempy2 = linkedObjectHead->y2;
	if( tempx2 < tempx1)
	{
		int32_t temp = tempx2;
		tempx2 = tempx1;
		tempx1 = temp;
	}
	//if( tempy2 < tempy1)
	//{
	//	int32_t temp = tempy2;
	//	tempy2 = tempy1;
	//	tempy1 = temp;
	//}
	int xSpan = tempx2 - tempx1;
	int ySpan = tempy2 - tempy1;
	for( int i = tempx1; i < tempx2; i++)
	{
		//interpolate y
		float xpercent = (i - (float)tempx1)/(float)xSpan;
		float yloc = tempy1 + (float)ySpan * xpercent;
		setPixelXY(i, yloc, 0x080808);
	}
	//For now, hardcode quadrant 1
	for( int i = 0; i < 32; i++ )
	{
		for( int j = 0; j < 8; j++ )
		{
			linkedViewport->setPixelXY( i, (7 - j), getPixelXY(i, j));
		}
	}
	linkedViewport->show();
}

void viewpage::setLayer( Layer* layerInput, uint8_t layerVar )
{
	layers[layerVar] = layerInput;
	
}

void viewpage::setLayerOffset( uint8_t layerVar, int32_t xOffsetVar, int32_t yOffsetVar )
{
	layers[layerVar]->xOffset = xOffsetVar;
	layers[layerVar]->yOffset = yOffsetVar;
	
}

//**********************************************************************//
//
//  Painting Tools
//
//**********************************************************************//

PaintTools::PaintTools( void )
{
	
}

void PaintTools::dot( Layer* inputLayer, float xVar, float yVar, int colorVar )
{
	//Identify the nearest pixel
	float nearestX = (int)xVar;
	float nearestY = (int)yVar;
	if(xVar > nearestX + 0.5)
	{
		nearestX += 1;
	}
	if(yVar > nearestY + 0.5)
	{
		nearestY += 1;
	}
	//Now, calculate the distance from a range around this center most pixel to all the others
	float bScalar;
	uint8_t R = colorVar >> 16;
	uint8_t G = colorVar >> 8;
	uint8_t B = colorVar;
	uint32_t outColor = 0;
	for(int j = nearestY - 1; j <= nearestY + 1; j++)
	{
		for(int i = nearestX - 1; i <= nearestX + 1; i++)
		{
			float distance = sqrt(pow(((float)i - xVar), 2) + pow(((float)j-yVar), 2));
			Serial.println(distance);
			//Create a brightness mask
			bScalar = pow(((2 - distance) / 2 ), 2);
			outColor = (uint8_t)( B * bScalar ) | (uint8_t)( G * bScalar ) << 8 | (uint8_t)( R * bScalar ) <<16;
			inputLayer->setPixelXY( i, j, outColor );
		}
	}
}






