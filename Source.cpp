#include <iostream>
#include "config_reader.h"


int main()
{
	ConfigReader cr;
	cr.read("trace.config");

	for (auto category : cr.m_config) {
		std::cout << category.first << std::endl;

		for (auto value : category.second) {
			std:: cout << "      " << value.first << ",    " << value.second << "#\n";
		}
	}

	return 0;
}