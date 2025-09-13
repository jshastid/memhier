#include <iostream>
#include "config_reader.h"
#include "cache_simulator.h"

int main()
{
	ConfigReader cr;
	cr.read("trace.config");

	Cache l1(cr["l1"]);

	return 0;
}