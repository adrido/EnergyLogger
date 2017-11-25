#include "ADCSingle.h"



ADCSingle::ADCSingle()
{
}


ADCSingle::~ADCSingle()
{
}

bool ADCSingle::open()
{
	// open device on /dev/i2c-1 
	// the default on Raspberry Pi B
	if ((fd = ::open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", fd);
		return false;
	}

	// connect to ads1115 as i2c slave
	if (ioctl(fd, I2C_SLAVE, ads_address) < 0) {
		printf("Error: Couldn't find device on address!\n");
		return false;
	}
	return true;
}

int ADCSingle::startConversation(PinSel select)
{
	// set config register and start conversion
	// ANC1 and GND, 4.096v, 128s/s
	writeBuf[0] = 1;    // config register is 1
	writeBuf[1] = 0b10000011 | static_cast<int>(select); // get the pinselect  
	                          // bit 15-8 0xD3
							  // bit 15 flag bit for single shot
							  // Bits 14-12 input selection:
							  // 100 ANC0; 101 ANC1; 110 ANC2; 111 ANC3
							  // Bits 11-9 Amp gain. Default to 010 here 001 P19
							  // Bit 8 Operational mode of the ADS1115.
							  // 0 : Continuous conversion mode
							  // 1 : Power-down single-shot mode (default)

	writeBuf[2] = 0b10000101; // bits 7-0  0x85
							  // Bits 7-5 data rate default to 100 for 128SPS
							  // Bits 4-0  comparator functions see spec sheet.

							  // begin conversion
	if (write(fd, writeBuf, 3) != 3) {
		perror("Write to register 1");
		exit(-1);
	}

	// wait for conversion complete
	// checking bit 15
	do {
		if (read(fd, writeBuf, 2) != 2) {
			perror("Read conversion");
			exit(-1);
		}
	} while ((writeBuf[0] & 0x80) == 0);



	// read conversion register
	// write register pointer first
	readBuf[0] = 0;   // conversion register is 0
	if (write(fd, readBuf, 1) != 1) {
		perror("Write register select");
		exit(-1);
	}

	// read 2 bytes
	if (read(fd, readBuf, 2) != 2) {
		perror("Read conversion");
		exit(-1);
	}

	// convert display results
	val = readBuf[0] << 8 | readBuf[1];

	if (val < 0)   val = 0;
	return val;
}

void ADCSingle::close()
{
	::close(fd);
}

float ADCSingle::toVolt(int val)
{
	return val * VPS;
}
