#include "hwLayer.h"
#include <math.h>

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
	uint32_t arraySize = width*height;
	layerMemory = new RGBA8[arraySize];   // Allocate memory for the variable
	for( int i = 0; i < (int)arraySize; i++ )
	{
		layerMemory[i].red = 0;
		layerMemory[i].green = 0;
		layerMemory[i].blue = 0;
		layerMemory[i].alpha = 0;
	}
	
}

void Layer::clear( void )
{
	uint32_t arraySize = width*height;
	for( int i = 0; i < (int)arraySize; i++ )
	{
		layerMemory[i].red = 0;
		layerMemory[i].green = 0;
		layerMemory[i].blue = 0;
		layerMemory[i].alpha = 0;
	}
	
}

void Layer::debugClear( void )
{
	uint8_t fill;
	uint32_t arraySize = width*height;
	for( int i = 0; i < (int)arraySize; i++ )
	{
		fill = ((float)i / (float)arraySize * 12) + 1;
		layerMemory[i].red = 255;
		layerMemory[i].green = 255;
		layerMemory[i].blue = 255;
		layerMemory[i].alpha = fill;
	}
	
}

void Layer::setPixelXY(int32_t x, int32_t y, RGBA8* color)
{
	if( x < xLowerLimit ) x = xLowerLimit;
	if( x > xUpperLimit ) x = xUpperLimit;
	if( y < yLowerLimit ) y = yLowerLimit;
	if( y > yUpperLimit ) y = yUpperLimit;
	//Data is organized as rows, offset width
	//offset x
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	layerMemory[(tempy * width + tempx)].red = color->red;
	layerMemory[(tempy * width + tempx)].green = color->green;
	layerMemory[(tempy * width + tempx)].blue = color->blue;
	layerMemory[(tempy * width + tempx)].alpha = color->alpha;
	
}

void Layer::getPixelXY(int32_t x, int32_t y, RGBA8* color)
{
	if( x < xLowerLimit ) x = xLowerLimit;
	if( x > xUpperLimit ) x = xUpperLimit;
	if( y < yLowerLimit ) y = yLowerLimit;
	if( y > yUpperLimit ) y = yUpperLimit;
	//Data is organized as rows, offset width
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	color->red = layerMemory[(tempy * width + tempx)].red;
	color->green = layerMemory[(tempy * width + tempx)].green;
	color->blue = layerMemory[(tempy * width + tempx)].blue;
	color->alpha = layerMemory[(tempy * width + tempx)].alpha;
	
	
}

void Layer::process( void )
{

}


////**********************************************************************//
////
////  pGLObject -- might be defunct
////
////**********************************************************************//
//
//pGLObject::pGLObject( void )
//{
//	x1 = 0;
//	x2 = 0;
//	y1 = 0;
//	y2 = 0;
//	
//}
//
//void pGLObject::setPoints( int32_t x1Var, int32_t y1Var, int32_t x2Var, int32_t y2Var )
//{
//	x1 = x1Var;
//	y1 = y1Var;
//	x2 = x2Var;
//	y2 = y2Var;
//	
//}



//**********************************************************************//
//
//  Viewpage
//
//**********************************************************************//


viewpage::viewpage( viewport* inputViewport )
{
	linkedViewport = inputViewport;
	xLowerLimit = -10;
	xUpperLimit =  42;
	yLowerLimit = -10;
	yUpperLimit =  42;
	width = xUpperLimit - xLowerLimit + 1;
	height = yUpperLimit - yLowerLimit + 1;
	uint32_t arraySize = width*height;
	viewpageMemory = new RGBA8[arraySize];   // Allocate memory for the variable
	for( int i = 0; i < (int)arraySize; i++ )
	{
		viewpageMemory[i].red = 0;
		viewpageMemory[i].green = 0;
		viewpageMemory[i].blue = 0;
		viewpageMemory[i].alpha = 255;
	}
	
}

void viewpage::clear( void )
{
	uint32_t arraySize = width*height;
	for( int i = 0; i < (int)arraySize; i++ )
	{
		viewpageMemory[i].red = 0;
		viewpageMemory[i].green = 0;
		viewpageMemory[i].blue = 0;
		viewpageMemory[i].alpha = 255;
	}
	
}

void viewpage::setPixelXY(int32_t x, int32_t y, RGBA8* color)
{
	if( x < xLowerLimit ) x = xLowerLimit;
	if( x > xUpperLimit ) x = xUpperLimit;
	if( y < yLowerLimit ) y = yLowerLimit;
	if( y > yUpperLimit ) y = yUpperLimit;	
	//Data is organized as rows, offset width
	//offset x
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	viewpageMemory[(tempy * width + tempx)].red = color->red;
	viewpageMemory[(tempy * width + tempx)].green = color->green;
	viewpageMemory[(tempy * width + tempx)].blue = color->blue;
	viewpageMemory[(tempy * width + tempx)].alpha = color->alpha;
	
}

void viewpage::getPixelXY(int32_t x, int32_t y, RGBA8* color)
{
	if( x < xLowerLimit ) x = xLowerLimit;
	if( x > xUpperLimit ) x = xUpperLimit;
	if( y < yLowerLimit ) y = yLowerLimit;
	if( y > yUpperLimit ) y = yUpperLimit;	
	//Data is organized as rows, offset width
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	color->red = viewpageMemory[(tempy * width + tempx)].red;
	color->green = viewpageMemory[(tempy * width + tempx)].green;
	color->blue = viewpageMemory[(tempy * width + tempx)].blue;
	color->alpha = viewpageMemory[(tempy * width + tempx)].alpha;
	
}

void viewpage::show( void )
{
	RGBA8 newPixel;
	RGBA8 underPixel;
	RGBA8 outPixel;
	//Process top layer
	layers[0]->process(); //Does nothing
	//Now copy the layer memory over
	for( int layi = 0; layi < 4; layi++ )
	{
		for( int j = layers[layi]->yLowerLimit; j <= layers[layi]->yUpperLimit; j++ )
		{
			for( int i = layers[layi]->xLowerLimit; i <= layers[layi]->xUpperLimit; i++ )
			{
				layers[layi]->getPixelXY(i, j, &newPixel);
				//old:
				//setPixelXY( i + layers[layi]->xOffset, j + layers[layi]->yOffset, &tempPixel);
				//new:
				getPixelXY(i + layers[layi]->xOffset, j + layers[layi]->yOffset, &underPixel);
				outPixel.red = (((uint16_t)newPixel.red * (uint16_t)newPixel.alpha) >> 8 ) + (((uint16_t)underPixel.red * ( (uint16_t)256 - newPixel.alpha)) >> 8 );
				outPixel.green = (((uint16_t)newPixel.green * (uint16_t)newPixel.alpha) >> 8 ) + (((uint16_t)underPixel.green * ( (uint16_t)256 - newPixel.alpha)) >> 8 );
				outPixel.blue = (((uint16_t)newPixel.blue * (uint16_t)newPixel.alpha) >> 8 ) + (((uint16_t)underPixel.blue * ( (uint16_t)256 - newPixel.alpha)) >> 8 );
				outPixel.alpha = 255;
				setPixelXY( i + layers[layi]->xOffset, j + layers[layi]->yOffset, &outPixel);
			}
		}
	}
	//For now, hardcode quadrant 1
	RGBA8 tempPixel;
	for( int i = 0; i < 32; i++ )
	{
		for( int j = 0; j < 8; j++ )
		{
			getPixelXY(i, j, &tempPixel);
			int32_t tempColor = tempPixel.blue;
			tempColor |= tempPixel.green << 8;
			tempColor |= tempPixel.red << 16;
			
			linkedViewport->setPixelXY( i, (7 - j), tempColor);
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

void PaintTools::dot( Layer* inputLayer, float xVar, float yVar, RGBA8* colorVar )
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
	RGBA8 outColor;
	outColor.red = colorVar->red;
	outColor.green = colorVar->green;
	outColor.blue = colorVar->blue;
	outColor.alpha = colorVar->alpha;
	for(int j = nearestY - 1; j <= nearestY + 1; j++)
	{
		for(int i = nearestX - 1; i <= nearestX + 1; i++)
		{
			float distance = sqrt(pow(((float)i - xVar), 2) + pow(((float)j-yVar), 2));
			//Serial.println(distance);
			//Create a brightness mask
			bScalar = pow(((2 - distance) / 2 ), 2);
			//Serial.println(bScalar);
			//Serial.println(outColor.red);
			outColor.red = colorVar->red;//(uint8_t)(bScalar * (float)colorVar->red);
			//Serial.println(outColor.red);
			outColor.green = colorVar->green;//bScalar * (float)colorVar->green;
			outColor.blue = colorVar->blue;//bScalar * (float)colorVar->blue;
			outColor.alpha = bScalar * (float)255;
			inputLayer->setPixelXY( i, j, &outColor );
		}
	}
}

void PaintTools::line( Layer* inputLayer, float x1Var, float y1Var, float x2Var, float y2Var, RGBA8* colorVar )
{
	//Swap to make sure (x,y)1 has a more negative x
	float temp;
	if( x2Var < x1Var )
	{
		temp = x2Var;
		x2Var = x1Var;
		x1Var = temp;

		temp = y2Var;
		y2Var = y1Var;
		y1Var = temp;

	}	
	
	//Identify the nearest pixel
	float xSpan = x2Var - x1Var;
	if( xSpan < 0 ) xSpan *= -1;
	float ySpan = y2Var - y1Var;
//	if( ySpan < 0 ) ySpan *= -1;
	float length = sqrt(pow(xSpan, 2) + pow(ySpan, 2));
	float xDelta = xSpan / length / 2 + 0.001;
	float yDelta = ySpan / length / 2 + 0.001;
	//Serial.println(xDelta);
//	float xDelta = xSpan / 5;
//	float yDelta = ySpan / 5;

	
	
	
	float xi = x1Var;
	float yi = y1Var;
	while( xi <= x2Var )
	{
	
		//Identify the nearest pixel
		float nearestX = (int)xi;
		float nearestY = (int)yi;
		if(xi > nearestX + 0.5)
		{
			nearestX += 1;
		}
		if(yi > nearestY + 0.5)
		{
			nearestY += 1;
		}
		//Now, calculate the distance from a range around this center most pixel to all the others
		float bScalar;
		RGBA8 outColor;
		RGBA8 layerColor;
		outColor.red = colorVar->red;
		outColor.green = colorVar->green;
		outColor.blue = colorVar->blue;
		outColor.alpha = colorVar->alpha;
		
		for(int j = nearestY - 1; j <= nearestY + 1; j++)
		{
			for(int i = nearestX - 1; i <= nearestX + 1; i++)
			{
				float distance = sqrt(pow(((float)i - xi), 2) + pow(((float)j-yi), 2));
				bScalar = pow(((2 - distance) / 2 ), 2);
				outColor.red = bScalar * (float)colorVar->red;
				outColor.green = bScalar * (float)colorVar->green;
				outColor.blue = bScalar * (float)colorVar->blue;
				outColor.alpha = 255;
				//read the existing pixel
				inputLayer->getPixelXY( i, j, &layerColor );
				if( layerColor.red > outColor.red ) outColor.red = layerColor.red;
				if( layerColor.green > outColor.green ) outColor.green = layerColor.green;
				if( layerColor.blue > outColor.blue ) outColor.blue = layerColor.blue;
				//output pixel
				inputLayer->setPixelXY( i, j, &outColor );
				
			}
		}
		//inputLayer->setPixelXY( xi, yi, colorVar );
		xi += xDelta;
		yi += yDelta;
	}
}

