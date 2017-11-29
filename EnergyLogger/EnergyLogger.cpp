#include "EnergyLogger.h"

bool EnergyLogger::running = true;

EnergyLogger::EnergyLogger()
{
	initDisplay();
}


EnergyLogger::~EnergyLogger()
{
	// Free PI GPIO ports
	display.close();
}

void EnergyLogger::initDisplay()
{
	if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64))
		exit(EXIT_FAILURE);

	display.begin();

	// init done
	display.clearDisplay();   // clears the screen  buffer
	display.display();   		// display it (clear display)

	display.setTextSize(2);
	//display.setTextColor(BLACK, WHITE); // 'inverted' text
	//display.setCursor(0, 0);
	//display.print("ADC out:  "); // First line
	//display.setTextColor(WHITE);

	//display.display();

	// Free PI GPIO ports
	//display.close();
}

void EnergyLogger::runLoop()
{
	while (running) 
	{
		auto start = std::chrono::high_resolution_clock::now();
		startADC();
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "RUNTIME of ADC: "
			<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
			<< " us " << std::endl;

		auto startDisplay = std::chrono::high_resolution_clock::now();
		drawDisplay();

		auto endDisplay = std::chrono::high_resolution_clock::now();
		std::cout << "RUNTIME of Display: "
			<< std::chrono::duration_cast<std::chrono::microseconds>(endDisplay - startDisplay).count()
			<< " us " << std::endl;
	}
}

void EnergyLogger::startADC()
{

	if (!adc.open())
		exit(EXIT_FAILURE);
	u1 = adc.startConversation(ADCSingle::PinSel::ANC0);
	u2 = adc.startConversation(ADCSingle::PinSel::ANC1);
	u3 = adc.startConversation(ADCSingle::PinSel::ANC2);
	adc.close();
}

void EnergyLogger::drawDisplay()
{
	//if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64))
	//	exit(EXIT_FAILURE);
	//display.begin();
	display.clearDisplay();   // clears the screen  buffer


	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.setCursor(0, 0);
	display.print("ADC out:  "); // First line
	display.setTextColor(WHITE);

	display.printf("U0: %.3fV", adc.toVolt(u1));
	display.printf("U1: %.3fV", adc.toVolt(u2));
	display.printf("U2: %.3fV", adc.toVolt(u3));
	display.display();

	// Free PI GPIO ports
	//display.close();
}

void EnergyLogger::signalHandler(int signal)
{
	std::cout << "Shutting down..." << std::endl;
	EnergyLogger::running = false;
}
