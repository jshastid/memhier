#include <iostream>
#include "config_reader.h"
#include "cache_simulator.h"

int main()
{
	ConfigReader cr;
	cr.read("trace.config");

	Cache l1(cr["l1"], &cr["pt"]);

	Address address = l1.segment_address("14c");
	std::cout << "tag: " << address.tag << std::endl;
	std::cout << "index: " << address.index << std::endl;
	std::cout << "offset: " << address.offset << std::endl;

	return 0;
}