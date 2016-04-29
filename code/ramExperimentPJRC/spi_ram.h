 /*
 * Mem read/write routines take from the PJRC Audio Library for Teensy 3.X.
 * 
 * Original header:
 *
 * * Audio Library for Teensy 3.X
 * * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 * *
 * * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * * open source software by purchasing Teensy or other PJRC products.
 * *
 * * Permission is hereby granted, free of charge, to any person obtaining a copy
 * * of this software and associated documentation files (the "Software"), to deal
 * * in the Software without restriction, including without limitation the rights
 * * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * * copies of the Software, and to permit persons to whom the Software is
 * * furnished to do so, subject to the following conditions:
 * *
 * * The above copyright notice, development funding notice, and this permission
 * * notice shall be included in all copies or substantial portions of the Software.
 * *
 * * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * * THE SOFTWARE.
 */ 
#ifndef SPI_RAM_H
#define SPI_RAM_H

#include "stdint.h"

class SpiRam
{
public:
	SpiRam( void );
	void initialize();
	void test( uint32_t *target );
	void dump( void );
private:
	void read(uint32_t address, uint32_t count, int16_t *data);
	void write(uint32_t address, uint32_t count, const int16_t *data);
	void fastRead8(uint32_t address, uint32_t count, uint8_t *data);
	void fastWrite8(uint32_t address, uint32_t count, uint8_t *data);

	uint32_t memory_begin;    // the first address in the memory we're using
	uint32_t memory_length;   // the amount of memory we're using

};

#endif
