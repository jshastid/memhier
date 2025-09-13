#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <sstream>


struct CacheBlock {
	std::vector<int> data;

	CacheBlock(size_t block_size) {
		data = std::vector<int>(block_size, 0);
	}
};

struct CacheEntry {
	int tag = -1;
	CacheBlock cb;

	CacheEntry(size_t block_size)
		: cb(block_size) {}
};

struct CacheSet {
	std::vector<CacheEntry> entries;

	CacheSet(size_t block_size, size_t set_size)
		: entries(set_size, CacheEntry(block_size)) {}
};

struct Address {
	int tag = 0, index = 0, offset = 0;
};


class Cache {
public:

	std::map<std::string, int> m_config;
	std::vector<CacheSet> m_cache;

	int n_sets = 0, set_size = 0, block_size = 0;
	bool write_through_no_write_allocate = false;

	// splits up the address into a tag, index, and offset
	Address segment_address(std::string str);

	// given a size, determines number of bits to index that size
	int calculate_bits_required(int n);

public:

	Cache(std::map<std::string, int> config);
};



Cache::Cache(std::map<std::string, int> config)
	: m_config(config) 
{
	n_sets = config["Number of sets"];
	set_size = config["Set size"];
	block_size = config["Line size"];
	write_through_no_write_allocate = config["Write through/no write allocate"];

	m_cache = std::vector<CacheSet>(n_sets, CacheSet(block_size, set_size));
};


Address Cache::segment_address(std::string str)
{
	Address result;
	std::stringstream ss(str);

	// convert the hex string value into an integer
	int address_value = 0;
	ss >> std::hex >> address_value;



	return Address();
}


int Cache::calculate_bits_required(int n)
{
	for (int i = 0; i != 32; ++i) {
		if (n >> i == 0) return i - 1;
	}

	std::cout << "bit calculator error: given improper value" << std::endl;
	return -1;
}