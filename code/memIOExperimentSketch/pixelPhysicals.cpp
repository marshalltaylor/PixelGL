//LEDs.  File format:
//[0] x offset
//[1] y offset
//[2] x scale
//[3] y scale
//[4] first pixel x
//[5] first pixel y
// ...
//
// This data is from 60 pixels arbitrarily mounted on a 0.5m x 0.5m board.
const float pixelPhysicalData[124] = {
0, 0, 0.2, 0.2, //offset and scaling
5,5, // first LED was at (5cm, 5cm)
12.25,5,
21,5.5,
27,10,
30.5,4,
36,10,
42.5,6,
41.25,12.5,
48,12,
52,5.5,
59.5,6,
62,13,
55.5,13,
59,21,
62,26.5,
56.5,32,
49,29,
42,29.5,
35,27,
40.5,23,
47,24,
54.5,24.5,
52.5,18.5,
44,18,
36,20,
30,16,
22,15,
14,18,
9.5,28.75,
10,37,
15,42,
19,36.5,
17,30.5,
20.5,24.25,
26.5,26,
24.25,35.5,
24.5,43.5,
25,51,
17.5,52,
11,54,
16.75,58.5,
25.5,59,
32,56.5,
30.25,48,
31,40.25,
37.5,35.5,
43.5,35.5,
50,38.5,
55,44.5,
53,51,
47.25,47.5,
41,43.5,
35.5,46.5,
37.5,53.5,
43,51,
48,56.5,
55.5,56.5,
61.5,52,
62,44,
56.5,37.5 };


//Real start of include
#include "pixelPhysicals.h"
#include <SPI.h>

#define SPIRAM_MOSI_PIN  11
#define SPIRAM_MISO_PIN  12
#define SPIRAM_SCK_PIN   13

#define SPIRAM_CS_PIN    5

#define SPISETTING SPISettings(25000000, MSBFIRST, SPI_MODE0)

PixelPhyStorage::PixelPhyStorage( void )
{
	openedForWrite = 0;
	openedForRead = 0;
	PIXEL_PHY_STORAGE_ADDRESS = 0;
	
}

int8_t PixelPhyStorage::openWrite( void )
{
	uint32_t addr = PIXEL_PHY_STORAGE_ADDRESS;
    
	SPI.beginTransaction(SPISETTING);
	digitalWriteFast(SPIRAM_CS_PIN, LOW);
	SPI.transfer16((0x02 << 8) | (addr >> 16));
	SPI.transfer16(addr & 0xFFFF);
	return 0;
}

int8_t PixelPhyStorage::openRead( void )
{
	//Set mode, send address
	uint32_t addr = PIXEL_PHY_STORAGE_ADDRESS;

	SPI.beginTransaction(SPISETTING);
	digitalWriteFast(SPIRAM_CS_PIN, LOW);
	SPI.transfer16((0x03 << 8) | (addr >> 16));
	SPI.transfer16(addr & 0xFFFF);
	return 0;
}

int8_t PixelPhyStorage::nextWrite( uint8_t *data )
{
	int8_t w = 0;
	w = *data;
	SPI.transfer(w);
	w++;
	SPI.transfer(w);
	w++;
	SPI.transfer(w);
	return 0;
}

int8_t PixelPhyStorage::nextRead( uint8_t *data )
{
	*data = (uint8_t)(SPI.transfer(0));
	data++;
	*data = (uint8_t)(SPI.transfer(0));
	data++;
	*data = (uint8_t)(SPI.transfer(0));
	return 0;
}

int8_t PixelPhyStorage::close( void )
{
	if( openedForWrite || openedForRead )
	{
		openedForWrite = 0;
		openedForRead = 0;
		digitalWriteFast(SPIRAM_CS_PIN, HIGH);
		SPI.endTransaction();
		return 0;
	}

	return 1;
}

