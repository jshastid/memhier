#include <iostream>
#include "config_reader.h"
#include "cache_simulator.h"

// TODO: add tlb


int main()
{
	// read the configuration file
	ConfigReader cr;
	cr.read("trace.config");

	PT page_table(cr["pt"]);
	TLB tlb(cr["pt"], cr["tlb"], page_table);

	std::string input;


	// testing
	while (std::cin >> input) {
		tlb.translate(input);
	}


	// create the cache levels
	Cache l1(cr["l1"], "L1", & cr["pt"]);
	Cache l2(cr["l2"], "L2", &cr["pt"]);

	if (cr.L2()) {
		l1.attach(&l2);
	}

	Address address = l1.segment_address("c84");
	std::cout << address.tag << ", " << address.index << ", " << address.offset << std::endl;

	l1.write("c84");
	l1.write("c84");
	return 0;
}