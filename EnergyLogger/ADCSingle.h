#pragma once
// http://www.bristolwatch.com/rpi/ads1115.html

#include <stdio.h>
#include <sys/types.h> // open
#include <sys/stat.h>  // open
#include <fcntl.h>     // open
#include <unistd.h>    // read/write usleep
#include <stdlib.h>    // exit
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
#include <vector>

class ADCSingle
{
public:
	// Bits 14-12 input selection:
	//           01110000
	// 100 ANC0; 101 ANC1; 110 ANC2; 111 ANC3
	enum class PinSel {
		ANC0 = 0b01000000,
		ANC1 = 0b01010000,
		ANC2 = 0b01100000,
		ANC3 = 0b01110000
	};

	ADCSingle();
	~ADCSingle();

	bool open();

	/*
	The resolution of the ADC in single ended
	mode we have 15 bit rather than 16 bit resolution,
	the 16th bit being the sign of the differential reading.
	*/
	int startConversation(PinSel select);
	std::vector<int> startConversationContinous(const PinSel select, const int numSamples);
	void close();

	static constexpr double VPS = 4.096 / 32768.0; //volts per step
	static float toVolt(int val);

	static double toVolt(double val);

private:
	int fd;
	// Note PCF8591 defaults to 0x48!
	const int ads_address = 0x48;
	int16_t val;

	uint8_t writeBuf[3];
	uint8_t readBuf[2];
};

