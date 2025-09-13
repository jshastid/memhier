#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>


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

	Address segment_address(std::string str);
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