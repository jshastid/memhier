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

	int m_virtual_address_size = 0, m_physical_address_size = 0;
	int n_sets = 0, set_size = 0, block_size = 0;
	bool write_through_no_write_allocate = false;

	// splits up the address into a tag, index, and offset
	Address segment_address(std::string str);

	// given a size, determines number of bits to index that size
	int calculate_bits_required(int n);

public:

	Cache(std::map<std::string, int> config, std::map<std::string, int>* page_table);
};



Cache::Cache(std::map<std::string, int> config, std::map<std::string, int>* page_table_ptr = nullptr)
	: m_config(config) 
{

	// get information for the cache size and specifications
	n_sets = config["Number of sets"];
	set_size = config["Set size"];
	block_size = config["Line size"];

	// get the virtual address size if there is a page size
	if (page_table_ptr) {
		int n_virtual_pages = (*page_table_ptr)["Number of virtual pages"];
		int page_size = (*page_table_ptr)["Page size"];

		int total_bytes_in_vmemory = n_virtual_pages * page_size;
		m_virtual_address_size = calculate_bits_required(total_bytes_in_vmemory);
	}

	// get the physical page size
	int n_physical_pages = (*page_table_ptr)["Number of physical pages"];
	int page_size = (*page_table_ptr)["Page size"];
	int total_bytes_in_memory = n_physical_pages * page_size;

	m_physical_address_size = calculate_bits_required(total_bytes_in_memory);

	// get the policy information
	write_through_no_write_allocate = config["Write through/no write allocate"];

	// create the cache
	m_cache = std::vector<CacheSet>(n_sets, CacheSet(block_size, set_size));
};


Address Cache::segment_address(std::string str)
{
	Address result;
	std::stringstream ss(str);

	// convert the hex string value into an integer
	unsigned int address_value = 0;
	ss >> std::hex >> address_value;

	unsigned int offset_bit_count = calculate_bits_required(block_size);
	unsigned int index_bit_count = calculate_bits_required(n_sets);
	unsigned int tag_bit_count = m_physical_address_size - index_bit_count - offset_bit_count;


	// get the offset value
	unsigned int tmp = (sizeof(int) * 8 - offset_bit_count);
	result.offset = (address_value << tmp) >> tmp;

	// get the index value
	tmp = (sizeof(int) * 8 - offset_bit_count - index_bit_count);
	result.index = (address_value << tmp) >> (tmp + offset_bit_count);

	// get the tag value
	result.tag = address_value >> (index_bit_count + offset_bit_count);

	return result;
}


int Cache::calculate_bits_required(int n)
{
	for (int i = 0; i != 32; ++i) {
		if (n >> i == 0) return i - 1;
	}

	std::cout << "bit calculator error: given improper value" << std::endl;
	return -1;
}