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
	for( int i = 0; i < arraySize; i++ )
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
	for( int i = 0; i < arraySize; i++ )
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
	for( int i = 0; i < arraySize; i++ )
	{
		fill = ((float)i / (float)arraySize * 20) + 1;
		layerMemory[i].red = 255;
		layerMemory[i].green = 255;
		layerMemory[i].blue = 255;
		layerMemory[i].alpha = fill;
	}
	
}

void Layer::setPixelXY(int32_t x, int32_t y, RGBA8* color)
{
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
	xLowerLimit = -20;
	xUpperLimit =  50;
	yLowerLimit = -20;
	yUpperLimit =  50;
	width = xUpperLimit - xLowerLimit + 1;
	height = yUpperLimit - yLowerLimit + 1;
	uint32_t arraySize = width*height;
	viewpageMemory = new RGBA8[arraySize];   // Allocate memory for the variable
	for( int i = 0; i < arraySize; i++ )
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
	for( int i = 0; i < arraySize; i++ )
	{
		viewpageMemory[i].red = 0;
		viewpageMemory[i].green = 0;
		viewpageMemory[i].blue = 0;
		viewpageMemory[i].alpha = 255;
	}
	
}

void viewpage::setPixelXY(int32_t x, int32_t y, RGBA8* color)
{
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
	
	//Data is organized as rows, offset width
	int32_t tempx = x - xLowerLimit;
	int32_t tempy = y - yLowerLimit;
	uint64_t tempColor = 0;
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
	for( int layi = 0; layi < 3; layi++)
	{
		for( int i = layers[layi]->xLowerLimit; i <= layers[layi]->xUpperLimit; i++ )
		{
			for( int j = layers[layi]->yLowerLimit; j <= layers[layi]->yUpperLimit; j++ )
			{
				layers[layi]->getPixelXY(i, j, &newPixel);
				//old:
				//setPixelXY( i + layers[layi]->xOffset, j + layers[layi]->yOffset, &tempPixel);
				//new:
				getPixelXY(i, j, &underPixel);
				outPixel.red = (((uint16_t)newPixel.red * (uint16_t)newPixel.alpha) >> 8 ) + (((uint16_t)underPixel.red * ( (uint16_t)256 - newPixel.alpha)) >> 8 );
				outPixel.green = (((uint16_t)newPixel.green * (uint16_t)newPixel.alpha) >> 8 ) + (((uint16_t)underPixel.green * ( (uint16_t)256 - newPixel.alpha)) >> 8 );
				outPixel.blue = (((uint16_t)newPixel.blue * (uint16_t)newPixel.alpha) >> 8 ) + (((uint16_t)underPixel.blue * ( (uint16_t)256 - newPixel.alpha)) >> 8 );
				outPixel.alpha = 255;
				setPixelXY( i + layers[layi]->xOffset, j + layers[layi]->yOffset, &outPixel);
			}
		}
	}
//mainPage[i].red = (((uint16_t)inputColorMatrix[i].red * (uint16_t)inputColorMatrix[i].alpha) >> 8 ) + (((uint16_t)mainPage[i].red * ( (uint16_t)256 - inputColorMatrix[i].alpha)) >> 8 );
	////Now copy the layer memory over
	//for( int i = layers[1]->xLowerLimit; i <= layers[1]->xUpperLimit; i++ )
	//{
	//	for( int j = layers[1]->yLowerLimit; j <= layers[1]->yUpperLimit; j++ )
	//	{
	//		layers[1]->getPixelXY(i, j, &tempPixel);
	//		setPixelXY( i + layers[1]->xOffset, j + layers[1]->yOffset, &tempPixel);
	//	}
	//}
	////Now copy the layer memory over
	//for( int i = layers[2]->xLowerLimit; i <= layers[2]->xUpperLimit; i++ )
	//{
	//	for( int j = layers[2]->yLowerLimit; j <= layers[2]->yUpperLimit; j++ )
	//	{
	//		layers[2]->getPixelXY(i, j, &tempPixel);
	//		setPixelXY( i + layers[2]->xOffset, j + layers[2]->yOffset, &tempPixel);
	//	}
	//}	
	
	////Process our 1 temp object, pGLObject
	////Step through x, find y
	//int32_t tempx1 = linkedObjectHead->x1;
	//int32_t tempy1 = linkedObjectHead->y1;
	//int32_t tempx2 = linkedObjectHead->x2;
	//int32_t tempy2 = linkedObjectHead->y2;
	//if( tempx2 < tempx1)
	//{
	//	int32_t temp = tempx2;
	//	tempx2 = tempx1;
	//	tempx1 = temp;
	//}
	////if( tempy2 < tempy1)
	////{
	////	int32_t temp = tempy2;
	////	tempy2 = tempy1;
	////	tempy1 = temp;
	////}
	//int xSpan = tempx2 - tempx1;
	//int ySpan = tempy2 - tempy1;
	//for( int i = tempx1; i < tempx2; i++)
	//{
	//	//interpolate y
	//	float xpercent = (i - (float)tempx1)/(float)xSpan;
	//	float yloc = tempy1 + (float)ySpan * xpercent;
	//	setPixelXY(i, yloc, 0x080808);
	//}
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



