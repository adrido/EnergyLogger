#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include "ADCSingle.h"
#include "ArduiPi_OLED_lib.h"
#include "ArduiPi_OLED.h"
#include "Database.h"


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
	Database db;

	int u1, u2, u3;

	const std::chrono::seconds interval{ 1 };
	static bool running;
};

