#pragma once

#include <iostream>
#include <chrono>

#include "ADCSingle.h"
#include "ArduiPi_OLED_lib.h"
#include "ArduiPi_OLED.h"


class EnergyLogger
{
public:
	EnergyLogger();
	~EnergyLogger();

	void initDisplay();
	void runLoop();
	void startADC();
	void drawDisplay();

	static void signalHandler(int signal);
private:
	ArduiPi_OLED display;
	ADCSingle adc;

	int u1, u2, u3;
	static bool running;
};

