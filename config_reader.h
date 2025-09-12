#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <unordered_map>



class ConfigReader {
public:

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
			m_config[active][parsed.first] = parsed.second;
		}
	}

	in.close();
}