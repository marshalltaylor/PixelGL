/* Memoryboard library code is placed under the MIT license
 * Copyright (c) 2015 Frank Bösing
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// Currently 23LC1024 SPI RAM only!

#ifndef _memory_SPI_RAM_h
#define _memory_SPI_RAM_h

/**************************/
// Memory Geometry Numbers

#define RAM_NUM_CHIPS         (1) //# of RAMs
#define RAM_CHIP_SIZE         (128*1024) //Size per RAM
#define RAM_SIZE              (RAM_CHIP_SIZE * RAM_NUM_CHIPS)
#define RAM_CHIP_SIZE_MASK    (RAM_CHIP_SIZE - 1)
#define RAM_SIZE_MASK         (RAM_SIZE - 1)

//End "Edit here"
/**************************/

#include <SPI.h>
#include <DMAChannel.h>
#include <arm_math.h>

#define MEMORY_CS0     8
#define MEMORY_SCK   	14
#define MEMORY_MOSI   	 7
#define MEMORY_MISO  	12

//RAM-Instructions
#define RAM_INSTR_READ      0x03
#define RAM_INSTR_WRITE     0x02
#define RAM_INSTR_EDIO      0x3B
#define RAM_INSTR_EQIO      0x38
#define RAM_INSTR_RSTIO     0xFF
#define RAM_INSTR_RDMR      0x05
#define RAM_INSTR_WRMR      0x01

#define RAM_BYTEMODE        0x00
#define RAM_PAGEMODE        0x80
#define RAM_SEQUENTIALMODE  0x40


#define MEMORYBOARD_PINMODE0	  
#define MEMORYBOARD_SETCS0(state) digitalWriteFast(MEMORYBOARD_CS0, state)
#define MEMORYBOARD_SETCS1(state)
#define MEMORYBOARD_SETCS2(state)
#if RAM_NUM_CHIPS > 1
#undef MEMORYBOARD_PINMODE1
#undef MEMORYBOARD_SETCS1
#define MEMORYBOARD_PINMODE1	  pinMode(MEMORYBOARD_CS1, OUTPUT)
#define MEMORYBOARD_SETCS1(state) digitalWriteFast(MEMORYBOARD_CS1, state)
#endif
#if RAM_NUM_CHIPS > 3
#undef MEMORYBOARD_PINMODE2
#undef MEMORYBOARD_SETCS2
#define MEMORYBOARD_PINMODE2	  {pinMode(MEMORYBOARD_CS2, OUTPUT);}
#define MEMORYBOARD_SETCS2(state) digitalWriteFast(MEMORYBOARD_CS2, state)
#endif


static DMAChannel dmatx(true);
static DMAChannel dmarx(true);
static DMAMEM uint8_t dummy;
static volatile bool done;




#endif


