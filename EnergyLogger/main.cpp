#include "EnergyLogger.h"
#include <csignal>

int main(int argc, char **argv)
{
	EnergyLogger logger;
	std::signal(SIGINT, EnergyLogger::signalHandler);
	logger.runLoop();
}
