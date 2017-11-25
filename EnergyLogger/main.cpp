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


