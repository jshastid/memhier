#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <unordered_map>


class ConfigReader {
private:
	bool m_virtual = false, m_tlb = false, m_l2 = false;

	// different category options for the config
	std::set<std::string> m_categories = {
		"Data TLB configuration",
		"Page Table configuration",
		"Data Cache configuration",
		"L2 Cache configuration",
		"Virtual addresses",
	};

	// takes in a str of form "name: value" and splits them
	std::pair<std::string, int> name_value_splicer(std::string str);

	// contains the configuration data
	std::unordered_map<std::string, std::map<std::string, int> > m_config;

public:

	// read the configuration file
	void read(std::string filename);

	// quick way to access config data
	std::map<std::string, int>& operator[](std::string str);


	// non-category specifications
	const bool Virtual() { return m_virtual; }
	const bool TLB() { return m_tlb; }
	const bool L2() { return m_l2; }
};


std::pair<std::string, int> ConfigReader::name_value_splicer(std::string str) 
{
	size_t seperator = str.find(':');

	if (seperator + 2 >= str.size() || seperator == -1) {
		std::cout << "error: name_value_splicer encountered an invalid string" << std::endl;
		exit(1);
	}

	std::string first = str.substr(0, seperator);
	std::string second_str = str.substr(seperator + 2, str.size());

	// handle the possible values for the value
	int second = 0;
	if (second_str == "y" || second_str == "n") {
		second = second_str == "y" ? 1 : 0;
	} else {
		second = std::stoi(second_str);
	}

	return std::make_pair(first, second);
}


void ConfigReader::read(std::string filename) 
{
	// current category that we are reading configs for
	std::string active;

	std::string line;
	std::ifstream in;

	in.open(filename, std::ios::in);

	// read every line of the file
	while (std::getline(in, line)) {

		// check to see if we are switching categories
		if (m_categories.find(line) != m_categories.end()) {
			active = line;
		} else {
			std::pair<std::string, int> parsed = name_value_splicer(line);

			if (parsed.first == "Virtual addresses") {
				m_virtual = parsed.second;
			}
			else if (parsed.first == "TLB") {
				m_tlb = parsed.second;
			}
			else if (parsed.first == "L2 cache") {
				m_l2 = parsed.second;
			}
			else {
				m_config[active][parsed.first] = parsed.second;
			}
		}
	}

	in.close();
}

std::map<std::string, int>& ConfigReader::operator[](std::string str) 
{
	if (str == "tlb") {
		return m_config["Data TLB configuration"];
	}
	else if (str == "pt") {
		return m_config["Page Table configuration"];
	}
	else if (str == "l1") {
		return m_config["Data Cache configuration"];
	}
	else if (str == "l2") {
		return m_config["L2 Cache configuration"];
	}
	else {
		std::cout << "operator error in ConfigReader";
	}
}