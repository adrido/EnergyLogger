#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

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
	static const constexpr float IpV = 10.05f; //1A/100mV
	static const constexpr float Ioff = 0.0f;

	static void signalHandler(int signal);
	static float toStrom(int u);
	double toStrom(double u);
	double getRMS(const ADCSingle::PinSel pin, const int offset);

	double getRMS(const std::vector<int> & arr, const int offset);

private:
	ArduiPi_OLED display;
	ADCSingle adc;
	Database db;

	double u1, u2, u3;

	const std::chrono::seconds interval{ 1 };
	static bool running;

	static const constexpr int historySize = 5;

	std::array<double, historySize> history1{ 0 };
	std::array<double, historySize> history2{ 0 };
	std::array<double, historySize> history3{ 0 };
	int writer = 0;

	double getMeanValue(const std::array<double, historySize>& arr) const;
};

