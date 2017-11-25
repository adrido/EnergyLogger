// http://www.bristolwatch.com/rpi/ads1115.html

#include <stdio.h>
#include <sys/types.h> // open
#include <sys/stat.h>  // open
#include <fcntl.h>     // open
#include <unistd.h>    // read/write usleep
#include <stdlib.h>    // exit
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions

int fd;
// Note PCF8591 defaults to 0x48!
int ads_address = 0x48;
int16_t val;

uint8_t writeBuf[3];
uint8_t readBuf[2];

float myfloat;

const float VPS = 4.096 / 32768.0; //volts per step


/*
The resolution of the ADC in single ended
mode we have 15 bit rather than 16 bit resolution,
the 16th bit being the sign of the differential reading.
*/
int test_adc() {

	// open device on /dev/i2c-1 
	// the default on Raspberry Pi B
	if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", fd);
		exit(1);
	}

	// connect to ads1115 as i2c slave
	if (ioctl(fd, I2C_SLAVE, ads_address) < 0) {
		printf("Error: Couldn't find device on address!\n");
		exit(1);
	}

	// set config register and start conversion
	// ANC1 and GND, 4.096v, 128s/s
	writeBuf[0] = 1;    // config register is 1
	writeBuf[1] = 0b11010011; // bit 15-8 0xD3
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

	myfloat = val * VPS; // convert to voltage

	printf("Values: HEX 0x%02x DEC %d reading %4.3f volts.\n",
		val, val, myfloat);

	close(fd);

	return 0;
}

#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"


/* ======================================================================
Function: main
Purpose : Main entry Point
Input 	: -
Output	: -
Comments:
====================================================================== */
int main(int argc, char **argv)
{
	test_adc();
	// Instantiate the display
	ArduiPi_OLED display;


	// I2C change parameters to fit to your LCD
	if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64))
		exit(EXIT_FAILURE);

	display.begin();

	// init done
	display.clearDisplay();   // clears the screen  buffer
	display.display();   		// display it (clear display)

	// text display tests
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.print("Hello, world!\n");
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.printf("%f\n", 3.141592);
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.printf("0x%8X\n", 0xDEADBEEF);
	display.display();
	display.clearDisplay();


	// Free PI GPIO ports
	display.close();

}


