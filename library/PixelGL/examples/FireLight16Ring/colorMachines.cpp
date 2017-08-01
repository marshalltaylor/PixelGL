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

#include "colorMachines.h"
#include "timeKeeper.h"

//****************************************************************************//
//
//  Transit
//
//****************************************************************************//
Transit::Transit( void )
{
  rAddValue = 0;
  bAddValue = 0;
  gAddValue = 0;
  aAddValue = 0;
  
  triggered = 0;
}

void Transit::setColor( RGBA8* inputColor, uint8_t steps )
{
	steps++;
	memcpy(&targetColor, inputColor, sizeof targetColor);
	//Set a new target number
	rAddValue = (inputColor->red - outputColor.red) / steps;
	if((inputColor->red - outputColor.red) > 0)
	{
		rAddValue++;
	}
	else
	{
		rAddValue--;
	}
	gAddValue = (inputColor->green - outputColor.green) / steps;
	if((inputColor->green - outputColor.green) > 0)
	{
		gAddValue++;
	}
	else
	{
		gAddValue--;
	}
	bAddValue = (inputColor->blue - outputColor.blue) / steps;
	if((inputColor->blue - outputColor.blue) > 0)
	{
		bAddValue++;
	}
	else
	{
		bAddValue--;
	}
	aAddValue = (inputColor->alpha - outputColor.alpha) / steps;
	if((inputColor->alpha - outputColor.alpha) > 0)
	{
		aAddValue++;
	}
	else
	{
		aAddValue--;
	}
	//Encode 0 as done
	//Serial.println("SetColorHit");
	//Serial.println(outputColor.red);
	//Serial.println(inputColor->red);
	//Serial.println(rAddValue);
	triggered = 1;
}

void Transit::tick( void )
{
	int16_t colorVar = 0;
	//red
	colorVar = outputColor.red + rAddValue;
	if( rAddValue > 0 )
	{
		if( colorVar >= targetColor.red )
		{
			colorVar = targetColor.red;
			rAddValue = 0;
		}
	}
	if( rAddValue < 0 )
	{
		if( colorVar <= targetColor.red )
		{
			colorVar = targetColor.red;
			rAddValue = 0;
		}
	}
	outputColor.red = colorVar;
	//green
	colorVar = outputColor.green + gAddValue;
	if( gAddValue > 0 )
	{
		if( colorVar >= targetColor.green )
		{
			colorVar = targetColor.green;
			gAddValue = 0;
		}
	}
	if( gAddValue < 0 )
	{
		if( colorVar <= targetColor.green )
		{
			colorVar = targetColor.green;
			gAddValue = 0;
		}
	}
	outputColor.green = colorVar;
	//blue
	colorVar = outputColor.blue + bAddValue;
	if( bAddValue > 0 )
	{
		if( colorVar >= targetColor.blue )
		{
			colorVar = targetColor.blue;
			bAddValue = 0;
		}
	}
	if( bAddValue < 0 )
	{
		if( colorVar <= targetColor.blue )
		{
			colorVar = targetColor.blue;
			bAddValue = 0;
		}
	}
	outputColor.blue = colorVar;
	//alpha
	colorVar = outputColor.alpha + aAddValue;
	if( aAddValue > 0 )
	{
		if( colorVar >= targetColor.alpha )
		{
			colorVar = targetColor.alpha;
			aAddValue = 0;
		}
	}
	if( aAddValue < 0 )
	{
		if( colorVar <= targetColor.alpha )
		{
			colorVar = targetColor.alpha;
			aAddValue = 0;
		}
	}
	outputColor.alpha = colorVar;

}

uint8_t Transit::done( void )
{
	if(triggered == 1)
	{
		if(( outputColor.red == targetColor.red )&&( outputColor.green == targetColor.green )&&( outputColor.blue == targetColor.blue )&&( outputColor.alpha == targetColor.alpha ))
		{
			triggered = 0;
			return 1;
		}
	}
	//Serial.println(triggered);
	//Serial.print("o: ");
	//Serial.print(outputColor.red);
	//Serial.print(" t: ");
	//Serial.print(targetColor.red);
	//Serial.print(" +: ");
	//Serial.println(rAddValue);
	//Serial.print("o: ");
	//Serial.print(outputColor.green);
	//Serial.print(" t: ");
	//Serial.print(targetColor.green);
	//Serial.print(" +: ");
	//Serial.println(gAddValue);
	//Serial.print("o: ");
	//Serial.print(outputColor.blue);
	//Serial.print(" t: ");
	//Serial.print(targetColor.blue);
	//Serial.print(" +: ");
	//Serial.println(bAddValue);
	//Serial.print("o: ");
	//Serial.print(outputColor.alpha);
	//Serial.print(" t: ");
	//Serial.print(targetColor.alpha);
	//Serial.print(" +: ");
	//Serial.println(aAddValue);
	return 0;
}

uint8_t Transit::busy( void )
{
	if(triggered == 1)
	{
		if(( outputColor.red == targetColor.red )&&( outputColor.green == targetColor.green )&&( outputColor.blue == targetColor.blue )&&( outputColor.alpha == targetColor.alpha ))
		{
			return 0;
		}
	}
	return 1;
}