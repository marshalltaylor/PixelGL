#ifndef PIXELPHYSICALS_H
#define PIXELPHYSICALS_H

#include "stdint.h"

class PixelPhyStorage
{
public:
	PixelPhyStorage( void );
	int8_t openWrite( void );
	int8_t openRead( void );
	int8_t nextWrite( uint8_t *data );
	int8_t nextRead( uint8_t *data );
	int8_t close( void );
private:
	uint8_t openedForWrite;
	uint8_t openedForRead;
	uint32_t PIXEL_PHY_STORAGE_ADDRESS;
};

#endif