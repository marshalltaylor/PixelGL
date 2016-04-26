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

#include <Arduino.h>
#include <SPI.h>
#include "spi_ram.h"

//#define INTERNAL_TEST

// Use these with the audio adaptor board  (should be adjustable by the user...)
#define SPIRAM_MOSI_PIN  11
#define SPIRAM_MISO_PIN  12
#define SPIRAM_SCK_PIN   13

#define SPIRAM_CS_PIN    5


SpiRam::SpiRam(void)
{

}


void SpiRam::initialize()
{
	memory_begin = 0x00000000;
	memory_length = 0x20000;
	
	//Make the CS pin
	pinMode(SPIRAM_CS_PIN, OUTPUT);
	digitalWriteFast(SPIRAM_CS_PIN, HIGH);
	
	SPI.setMOSI(SPIRAM_MOSI_PIN);
	SPI.setMISO(SPIRAM_MISO_PIN);
	SPI.setSCK(SPIRAM_SCK_PIN);

	SPI.begin();
//	zero(0, memory_length);

}

#define SPISETTING SPISettings(25000000, MSBFIRST, SPI_MODE0)

void SpiRam::read(uint32_t offset, uint32_t count, int16_t *data)
{
	uint32_t addr = memory_begin + offset;

	SPI.beginTransaction(SPISETTING);
	digitalWriteFast(SPIRAM_CS_PIN, LOW);
	SPI.transfer16((0x03 << 8) | (addr >> 16));
	SPI.transfer16(addr & 0xFFFF);
	while (count) {
		*data++ = (int16_t)(SPI.transfer16(0));
		count--;
	}
	digitalWriteFast(SPIRAM_CS_PIN, HIGH);
	SPI.endTransaction();
}

void SpiRam::write(uint32_t offset, uint32_t count, const int16_t *data)
{
	uint32_t addr = memory_begin + offset;
    
	SPI.beginTransaction(SPISETTING);
	digitalWriteFast(SPIRAM_CS_PIN, LOW);
	SPI.transfer16((0x02 << 8) | (addr >> 16));
	SPI.transfer16(addr & 0xFFFF);
	while (count) {
		int16_t w = 0;
		if (data) w = *data++;
		SPI.transfer16(w);
		count--;
	}
	digitalWriteFast(SPIRAM_CS_PIN, HIGH);
	SPI.endTransaction();

}

void SpiRam::fastRead8(uint32_t offset, uint32_t count, uint8_t *data)
{
	uint32_t addr = memory_begin + offset;

	SPI.beginTransaction(SPISETTING);
	digitalWriteFast(SPIRAM_CS_PIN, LOW);
	SPI.transfer16((0x03 << 8) | (addr >> 16));
	SPI.transfer16(addr & 0xFFFF);
	while (count) {
		*data++ = (uint8_t)(SPI.transfer(0));
		count--;
	}
	digitalWriteFast(SPIRAM_CS_PIN, HIGH);
	SPI.endTransaction();
}

void SpiRam::fastWrite8(uint32_t offset, uint32_t count, uint8_t *data)
{
	uint32_t addr = memory_begin + offset;
    
	SPI.beginTransaction(SPISETTING);
	digitalWriteFast(SPIRAM_CS_PIN, LOW);
	SPI.transfer16((0x02 << 8) | (addr >> 16));
	SPI.transfer16(addr & 0xFFFF);
	while (count) {
		SPI.transfer(*data++);
		count--;
	}
	digitalWriteFast(SPIRAM_CS_PIN, HIGH);
	SPI.endTransaction();

}


//This uses print
void SpiRam::test( uint32_t *usTicks )
{
	uint32_t failures = 0;
	Serial.println("Test began.");

	int16_t temp1 = 0x1002;  //Make a variable with distinct pattern
	int16_t *refPtr;  //Make unassigned pointer
	refPtr = &temp1;  //pointer (no star) = address of
	Serial.print("Address of usTicks: ");
	Serial.println(*refPtr, HEX);  //Print the thing the pointer points to
	write(0,1,refPtr);
	temp1 = 0x3004;
	write(2,1,refPtr);
	temp1 = 0x0;
	read(1,1, refPtr);
	
	uint32_t time1, time2, time3;
	
	//Tires are warm. approach the starting line
	time1 = *usTicks;
	temp1 = 0xF5AA;
	Serial.print("Start usTicks count (time1): ");
	Serial.println(time1);
	Serial.print("Writing... ");
	for( int i = 0; i < 0x20000; i += 2 )
	{

		write(i,1,refPtr);
	}
	Serial.println("Done");
	time2 = *usTicks;
	Serial.print("usTicks count (time2): ");
	Serial.println(time2);
	Serial.print("Reading... ");
	for( int i = 0; i < 0x20000; i += 2 )
	{
		*refPtr = 0;
		read(i,1,refPtr);
	}
	time3 = *usTicks;
	Serial.println("Done");
	Serial.print("usTicks count (time3): ");
	Serial.println(time3);

	//Now assess data
	for( int i = 0; i < 0x20000; i += 2 )
	{
		*refPtr = 0;
		read(i,1,refPtr);
		if( i < 5 )
		{
			//Note HEX doesn't handle negative well.  Trust int
			Serial.println(*refPtr, HEX);
		}
		if((uint16_t)*refPtr != 0xF5AA) failures++;
	}
	Serial.print("Write time: ");
	Serial.println(time2 - time1);
	Serial.print("Read time: ");
	Serial.println(time3 - time2);
	Serial.print("Total time: ");
	Serial.println(time3 - time1);
	Serial.print("Number of failures: ");
	Serial.println(failures);
	
	
	
	//Create 0x100 worth of bytes
	uint8_t temp[256];
	for( int i = 0; i < 256; i++ )
	{
		temp[i] = i;
	}
	uint8_t *refPtr8 = &temp[0];
	
	//Do the next test
	time1 = *usTicks;

	Serial.println(*usTicks);
	Serial.print("Writing... ");
	for( int i = 0; i < 0x200; i++ )
	{

		fastWrite8(i,256,refPtr8);
	}
	Serial.println("Done");
	time2 = *usTicks;
	Serial.print("Reading... ");
	for( int i = 0; i < 0x200; i++ )
	{
		*refPtr8 = 0;
		fastRead8(i,256,refPtr8);
	}
	time3 = *usTicks;
	Serial.println("Done");
	
	//Now assess data
	for( int i = 0; i < 256; i++ )
	{
		temp[i] = 0xDB;
	}
	for( int i = 0; i < 0x200; i++ )
	{
		fastRead8(i,256,refPtr8);
		if( i < 5 )
		{
			//Note HEX doesn't handle negative well.  Trust int
			Serial.println(*refPtr8, HEX);
			Serial.println(*refPtr8+1, HEX);
			Serial.println(*refPtr8+2, HEX);
			Serial.println(*refPtr8+3, HEX);
		}
		if((uint16_t)*refPtr8 != 0xAAF5) failures++;
	}
	Serial.print("Write time: ");
	Serial.println(time2 - time1);
	Serial.print("Read time: ");
	Serial.println(time3 - time2);
	Serial.print("Total time: ");
	Serial.println(time3 - time1);
	Serial.print("Number of failures: ");
	Serial.println(failures);
	
	
	
	
}


