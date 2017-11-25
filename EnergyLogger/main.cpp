#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"
#include "ADCSingle.h"

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
	// Instantiate the ADC
	ADCSingle adc;

	if(!adc.open())
		exit(EXIT_FAILURE);
	int u1 = adc.startConversation(ADCSingle::PinSel::ANC0);
	int u2 = adc.startConversation(ADCSingle::PinSel::ANC1);
	int u3 = adc.startConversation(ADCSingle::PinSel::ANC2);
	adc.close();



	// I2C change parameters to fit to your LCD
	if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64))
		exit(EXIT_FAILURE);

	display.begin();

	// init done
	display.clearDisplay();   // clears the screen  buffer
	display.display();   		// display it (clear display)

	// text display tests
	display.setTextSize(2);
	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.setCursor(0, 0);
	display.print("ADC out:  ");
	display.setTextColor(WHITE);
	display.printf("U0: %.3fV", adc.toVolt(u1));
	display.printf("U1: %.3fV", adc.toVolt(u2));
	display.printf("U2: %.3fV", adc.toVolt(u3));
	display.display();
	display.clearDisplay();


	// Free PI GPIO ports
	display.close();

}


