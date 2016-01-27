#include "Arduino.h"



void Memory_SPI_RAM::config()
{
    SPISettings(20000000, MSBFIRST, SPI_MODE0 ) //Speed. 23LC1024 = 20MHz max, see Datasheet.
	pinMode(MEMORY_CS0, OUTPUT);

}

void memoryboard_init(void)
{

	MEMORYBOARD_PINMODE0;
	MEMORYBOARD_PINMODE1;
	MEMORYBOARD_PINMODE2;
	CSRELEASE;

	SPI.setMOSI(MEMORYBOARD_MOSI);
	SPI.setMISO(MEMORYBOARD_MISO);
	SPI.setSCK(MEMORYBOARD_SCK);
	SPI.begin();

	for (int num=1; num <= RAM_NUM_CHIPS; num++)
		ram_writeRDMR( RAM_SEQUENTIALMODE, num);

	//DMA
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX;
	SIM_SCGC7 |= SIM_SCGC7_DMA;

	//dmatx.begin(true);
	dmatx.disable();
	dmatx.destination(*(volatile uint8_t*)&SPI0_PUSHR);
	dmatx.triggerAtHardwareEvent( DMAMUX_ENABLE | DMAMUX_SOURCE_SPI0_TX );
	dmatx.disableOnCompletion();

	//dmarx.begin(true);
	dmarx.disable();
	dmarx.attachInterrupt(rxIsr);
	dmarx.interruptAtCompletion();
	dmarx.source(*(volatile uint8_t*)&SPI0_POPR);
	dmarx.triggerAtHardwareEvent( DMAMUX_SOURCE_SPI0_RX );
	dmarx.disableOnCompletion();

	done = false;
}

void Memory_SPI_RAM::csAssert()
{
    MEMORYBOARD_SETCS0(HIGH);
	MEMORYBOARD_SETCS1(LOW);
	MEMORYBOARD_SETCS2(LOW);

}


void Memory_SPI_RAM::csDeassert()
{
	MEMORYBOARD_SETCS0(LOW)
	MEMORYBOARD_SETCS1(LOW)
	MEMORYBOARD_SETCS2(LOW)
}


//Read Mode Register
int Memory_SPI_RAM::readRDMR(const int RamNo)
{
	int ret;
	SPI.beginTransaction(MEMORYBOARD_SPICONFIG);
	csassert(RamNo);
	SPI.transfer(RAM_INSTR_RDMR);
	ret = SPI.transfer(0);
	ram_stop();
	return ret;
}

//Write Mode Register
void Memory_SPI_RAM::writeRDMR(uint8_t mode, const int RamNo)
{
	SPI.beginTransaction(MEMORYBOARD_SPICONFIG);
	csassert(RamNo);
	SPI.transfer16(RAM_INSTR_WRMR << 8 | mode);
	ram_stop();
}

static inline
void Memory_SPI_RAM::startRead(const uint8_t cs, const uint32_t addr)
{
	uint32_t tmp;
	SPI.beginTransaction(MEMORYBOARD_SPICONFIG);
	csassert(cs);
	tmp = (RAM_INSTR_READ << 24) | (addr & RAM_CHIP_SIZE_MASK);
	SPI.transfer16( tmp>>16 );
	SPI.transfer16( tmp );
}

static inline
void Memory_SPI_RAM::startWrite(const uint8_t cs, const uint32_t addr)
{
	uint32_t tmp;
	SPI.beginTransaction(MEMORYBOARD_SPICONFIG);
	csassert(cs);
	tmp = (RAM_INSTR_WRITE << 24) | (addr & RAM_CHIP_SIZE_MASK);
	SPI.transfer16( tmp>>16 );
	SPI.transfer16( tmp );
}

void Memory_SPI_RAM::read16(const uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
	if (len == 0) return;
	ram_startRead(cs, addr);

	//align with 8 BIT Transfer
	if ((uintptr_t)buf & 0x01) {
		*buf++ = SPI.transfer(0);
		len--;
	}

	//16BIT Transfer
	while(len > 1) {
		*(uint16_t*)buf = __REV16(SPI.transfer16(0));
		buf += 2;
		len -= 2;
	}

  //8BIT Transfer
	while(len > 0) {
		*buf++ = SPI.transfer(0);
		len--;
	}

	ram_stop();
}


void Memory_SPI_RAM::write16(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
	if (len == 0) return;
	ram_startWrite(cs, addr);

   //align with 8 BIT Transfer
	if ((uintptr_t)buf & 0x01) {
		SPI.transfer(*buf++);
		len--;
	}

	//16BIT Transfer
	while(len > 1) {
		SPI.transfer16(__REV16(*(uint16_t*)buf));
		buf += 2;
		len -= 2;
	}

	//8BIT Transfer
	while(len > 0) {
		SPI.transfer(*buf++);
		len--;
	}

	ram_stop();
}


static inline
void ram_stop(void)
{
	CSRELEASE;
	SPI.endTransaction();
}





//waitFifoEmpty taken from ILI9341_t3.h, (c) Paul Stoffregen
inline void waitFifoEmpty(void) {
	uint32_t sr;
	uint32_t tmp __attribute__((unused));
	do {
		sr = KINETISK_SPI0.SR;
		if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
	} while ((sr & 0xF0F0) > 0);             // wait both RX & TX empty
}

static void rxIsr(void)
{
	dmarx.clearInterrupt();
	done = true;
}

inline 
void ram_waitDMA(void)
{ //This does not work with 144 MHZ. WHY? TODO.
	while(!done) {asm volatile("wfi");};
	ram_stop();
}

inline
void ram_readDMA_noWait(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
    ram_startRead(cs, addr);
    dmatx.source(*(uint8_t*)&dummy);
	dmatx.transferCount(len);
	dmarx.destinationBuffer(buf, len);
	done = false;
	waitFifoEmpty();
    SPI0_SR = 0xFF0F0000;
	SPI0_MCR |= SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF;
	SPI0_RSER = SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS | SPI_RSER_RFDF_RE | SPI_RSER_RFDF_DIRS;
	dmatx.clearComplete();
	dmarx.enable();
	dmatx.enable();
}

void ram_readDMA(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
    ram_readDMA_noWait(cs, buf, addr, len);
	ram_waitDMA();
}

void ram_writeDMA_noWait(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
    ram_startWrite(cs, addr);
	dmatx.sourceBuffer(buf, len);
	dmarx.destination(*(uint8_t*)&dummy);
	dmarx.transferCount(len);
	done = false;
	waitFifoEmpty();
	SPI0_RSER = SPI_RSER_TFFF_RE | SPI_RSER_TFFF_DIRS | SPI_RSER_RFDF_RE | SPI_RSER_RFDF_DIRS;
	dmatx.clearComplete();
	dmarx.enable();
	dmatx.enable();
}

void ram_writeDMA(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
    ram_writeDMA_noWait(cs, buf, addr, len);
    ram_waitDMA();
}


void ram_read(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
    if ( len == 0 ) return;
    if ( len <= 16 ) //short transfers are faster without dma
      ram_read16(cs, buf, addr, len);
    else
      ram_readDMA(cs, buf, addr, len);
}

void ram_write(uint8_t cs, uint8_t *buf, const uint32_t addr, size_t len)
{
    if ( len == 0 ) return;
	if ( len <= 16 ) //short transfers are faster without dma
      ram_write16(cs, buf, addr, len);
    else
      ram_writeDMA(cs, buf, addr, len);
}
