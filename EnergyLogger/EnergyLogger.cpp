#include "EnergyLogger.h"
#include <random>
#include <array>
#include <cmath>

bool EnergyLogger::running = true;

EnergyLogger::EnergyLogger()
{
	u1 = u2 = u3 = 0;
	initDisplay();
}


EnergyLogger::~EnergyLogger()
{
	display.clearDisplay();   // clears the screen  buffer
	display.display();
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
}

void EnergyLogger::runLoop()
{
	while (running) 
	{
		// ADC
		auto start = std::chrono::high_resolution_clock::now();
		startADC();
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "RUNTIME of ADC: "
			<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
			<< " us " << std::endl;

		// Display
		auto startDisplay = std::chrono::high_resolution_clock::now();
		drawDisplay();
		auto endDisplay = std::chrono::high_resolution_clock::now();
		std::cout << "RUNTIME of Display: "
			<< std::chrono::duration_cast<std::chrono::microseconds>(endDisplay - startDisplay).count()
			<< " us" << std::endl;

		// MySQL
		auto startSQL = std::chrono::high_resolution_clock::now();
		db.save(toStrom(u1), toStrom(u2), toStrom(u3));
		auto endSQL = std::chrono::high_resolution_clock::now();
		std::cout << "RUNTIME of SQL: "
			<< std::chrono::duration_cast<std::chrono::microseconds>(endSQL - startSQL).count()
			<< " us" << std::endl;

		std::chrono::microseconds cTime = std::chrono::duration_cast<std::chrono::microseconds>(endSQL - start);
		std::cout << "Cycletime: "
			<< cTime.count()
			<< " us" << std::endl;
		std::this_thread::sleep_for(interval - cTime);
	}
}

void EnergyLogger::startADC()
{
	if (!adc.open())
		exit(EXIT_FAILURE);

	history1[writer] = getRMS(ADCSingle::PinSel::ANC0, 18663);
	history2[writer] = getRMS(ADCSingle::PinSel::ANC1, 18625);
	history3[writer] = getRMS(ADCSingle::PinSel::ANC2, 18625);
	//history1[writer] = getRMS(adc.startConversationContinous(ADCSingle::PinSel::ANC0, 34), 18663);
	//history2[writer] = getRMS(adc.startConversationContinous(ADCSingle::PinSel::ANC1, 34), 18625);
	//history3[writer] = getRMS(adc.startConversationContinous(ADCSingle::PinSel::ANC2, 34), 18625);
	
	writer++;
	if (writer >= historySize)
		writer = 0;

	//summeUQadrat1 = Messwerte1[0] * Messwerte1[0];
	//double sumoffset = 0.0;
	//for (int i = 0; i < 2000; i++)
	//	sumoffset += adc.startConversation(ADCSingle::PinSel::ANC0);

	//u3 = sumoffset / 2000;
	
	u1 = getMeanValue(history1);
	u2 = getMeanValue(history2);
	u3 = getMeanValue(history3);

	adc.close();
}

void EnergyLogger::drawDisplay()
{
	display.clearDisplay();   // clears the screen  buffer


	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.setCursor(0, 0);
	display.print("E-Logger  "); // First line
	display.setTextColor(WHITE);

	display.printf("I1:% 2.3fA", toStrom(u1));
	display.printf("I2:% 2.3fA", toStrom(u2));
	display.printf("I3:% 2.3fA", toStrom(u3));


	display.display();

	// Free PI GPIO ports
	//display.close();
}

void EnergyLogger::signalHandler(int signal)
{
	std::cout << "Shutting down..." << std::endl;
	EnergyLogger::running = false;
}


float EnergyLogger::toStrom(int u)
{
	return (ADCSingle::toVolt(u) * IpV) - Ioff;
}

double EnergyLogger::toStrom(double u)
{
	return (ADCSingle::toVolt(u) * IpV) - Ioff;
}

double EnergyLogger::getRMS(const ADCSingle::PinSel pin, const int offset)
{
	const int zyklen = 34;
	std::array<int, zyklen> messwerte;
	double summeUQuadrat = 0.0;

	for (int &i : messwerte) {
		i = adc.startConversation(pin);
	}
	for (int i : messwerte) {
		const int u = i - offset;
		summeUQuadrat += u * u;
	}
	return std::sqrt(summeUQuadrat / zyklen);
}

double EnergyLogger::getRMS(const std::vector<int> &messwerte, const int offset)
{
	double summeUQuadrat = 0.0;

	for (const int i : messwerte)
	{
		const int u = i - offset;
		summeUQuadrat += u * u;
	}
	
	return std::sqrt(summeUQuadrat / messwerte.size());
}

double EnergyLogger::getMeanValue(const std::array<double, historySize> &arr) const
{
	double summe = 0.0;
	for (const double d : arr) {
		summe += d;
	}
	return summe / arr.size();
}
