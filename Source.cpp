#include <iostream>
#include "config_reader.h"
#include "cache_simulator.h"

// TODO: add tlb, finish write, make print statements, make sure print prints own name


int main()
{
	// read the configuration file
	ConfigReader cr;
	cr.read("trace.config");


	// create the cache levels
	Cache l1(cr["l1"], "L1", & cr["pt"]);
	Cache l2(cr["l2"], "L2", &cr["pt"]);

	if (cr.L2()) {
		l1.attach(&l2);
	}

	l1.read("c84");

	Address address = l1.segment_address("14c");
	std::cout << "tag: " << address.tag << std::endl;
	std::cout << "index: " << address.index << std::endl;
	std::cout << "offset: " << address.offset << std::endl;

	return 0;
}