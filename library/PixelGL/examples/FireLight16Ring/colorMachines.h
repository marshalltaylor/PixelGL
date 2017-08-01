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
#ifndef COLORMACHINES_H_INCLUDED
#define COLORMACHINES_H_INCLUDED

#include "stdint.h"
#include "PixelGL.h"

class Transit
{
public:
  Transit( void );
  
  RGBA8 outputColor; //Output
  RGBA8 targetColor;

  int16_t rAddValue; // Number to add each second, becomes 0 for hold
  int16_t bAddValue; // Number to add each second, becomes 0 for hold
  int16_t gAddValue; // Number to add each second, becomes 0 for hold
  int16_t aAddValue; // Number to add each second, becomes 0 for hold

  uint8_t triggered;

  void setColor( RGBA8* inputColor, uint8_t steps );
  void tick( void );
  uint8_t done( void );
  uint8_t busy( void );
};


#endif