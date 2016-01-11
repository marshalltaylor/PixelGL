#ifndef LOOPERPANEL_H_INCLUDED
#define LOOPERPANEL_H_INCLUDED

#include <OctoWS2811.h>

class viewport : public OctoWS2811
{
public:
	viewport(uint32_t hwL_numPerStrip, void *hwL_frameBuf, void *hwL_drawBuf, uint8_t hwL_config = WS2811_GRB, uint32_t hwL_width = 16, uint32_t hwL_height = 10) : OctoWS2811(hwL_numPerStrip, hwL_frameBuf, hwL_drawBuf, hwL_config), width(hwL_width), height(hwL_height) {};
	void setPixelXY(uint32_t x, uint32_t y, int color);
//private:
	uint32_t width;
	uint32_t height;
};

class viewpage
{
public:
	viewpage( viewport* );
	void setPixelXY( int32_t x, int32_t y, int color );
	int64_t getPixelXY( int32_t x, int32_t y );
	void show( void );
//private
	int32_t xLowerLimit;
	int32_t xUpperLimit;
	int32_t yLowerLimit;
	int32_t yUpperLimit;
	int32_t width;
	int32_t height;
	uint8_t* viewpageMemory;
	viewport* linkedViewport;
};


#endif