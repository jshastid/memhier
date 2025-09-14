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
	int tag = -1, dirty = 0;
	unsigned int used_time = -1;

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
	std::string name;

	// this is used to help calculate LRU
	int timer = 0;

	std::map<std::string, int> m_config;
	std::vector<CacheSet> m_cache;
	Cache* m_next_layer = nullptr;

	int m_virtual_address_size = 0, m_physical_address_size = 0;
	int n_sets = 0, set_size = 0, block_size = 0, page_size = 0;
	bool write_through_no_write_allocate = false;

	// splits up the address into a tag, index, and offset
	Address segment_address(std::string str);

	// given a size, determines number of bits to index that size
	int calculate_bits_required(int n);

	// check to see if a value exists in the current level of cache
	bool check_if_exists(int tag, int index);

	// finds a place to put tag in cache set index. uses LRU eviction if needed
	void find_place(std::string _address);

	void set_dirty_bit(int index, int tag);

public:
	// reads from memory at the specified address
	void read(std::string address);

	// writes to memory at a specific address
	void write(std::string address);

	// connect the next level of cache. for instance L1.attach(L2); L2.attach(L3);
	void attach(Cache* next_level);

	Cache(std::map<std::string, int> config, std::string _name, std::map<std::string, int>* page_table);
};



Cache::Cache(std::map<std::string, int> config, std::string _name, std::map<std::string, int>* page_table_ptr = nullptr)
	: m_config(config), name(_name)
{
	name = _name;
	// get information for the cache size and specifications
	n_sets = config["Number of sets"];
	set_size = config["Set size"];
	block_size = config["Line size"];

	// get the virtual address size if there is a page size
	if (page_table_ptr) {
		int n_virtual_pages = (*page_table_ptr)["Number of virtual pages"];
		page_size = (*page_table_ptr)["Page size"];

		int total_bytes_in_vmemory = n_virtual_pages * page_size;
		m_virtual_address_size = calculate_bits_required(total_bytes_in_vmemory);
	}

	// get the physical page size
	int n_physical_pages = (*page_table_ptr)["Number of physical pages"];
	page_size = (*page_table_ptr)["Page size"];
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


bool Cache::check_if_exists(int tag, int index)
{
	// select the set which the value should be
	CacheSet& cs = m_cache[index % n_sets];
	
	for (size_t i = 0; i != cs.entries.size(); ++i) {
		if (cs.entries[i].tag == tag) {
			
			// used for calculating LRU
			cs.entries[i].used_time = timer;
			timer++;

			return true;
		}
	}
	return false;
}


void Cache::read(std::string _address)
{
	Address address = segment_address(_address);
	bool exists = check_if_exists(address.tag, address.index);

	if (exists) {
		std::cout << "hit in " << name << " at address " << _address << std::endl;
		return;
	}

	std::cout << "miss in " << name << " at address " << _address << std::endl;

	// read the value from the next layer
	if (m_next_layer) {
		m_next_layer->read(_address);
	} else {
		std::cout << "Read from memory" << std::endl;
	}

	find_place(_address);
}


void Cache::find_place(std::string _address)
{
	Address address = segment_address(_address);
	CacheSet& cs = m_cache[address.index];

	// records which value in the set was least recently used LRU
	unsigned int lowest_value = 0, lowest_index = 0;

	for (size_t i = 0; i != cs.entries.size(); ++i) {
		
		// check to see if an empty spot was found
		if (cs.entries[i].tag == -1) {
			cs.entries[i].tag = address.tag;
		}

		// update LRU meta if needed
		if (cs.entries[i].used_time < lowest_value) {
			lowest_value = cs.entries[i].used_time;
			lowest_index = i;
		}
	}

	// handle write back policy dirty bit
	if (cs.entries[lowest_index].dirty) {
		m_next_layer->write(_address);
		cs.entries[lowest_index].dirty = 0;
	}

	// no emtpy spots found
	cs.entries[lowest_index].tag = address.tag;
	cs.entries[lowest_index].used_time = timer;
	timer++;
}


void Cache::attach(Cache* next_layer)
{
	m_next_layer = next_layer;
}


void Cache::write(std::string _address)
{
	Address address = segment_address(_address);
	bool exists = check_if_exists(address.tag, address.index);

	if (exists && write_through_no_write_allocate) {
		std::cout << "wrote to " << _address << " in layer " << name << std::endl;

		// write through is enabled, write to the next level
		m_next_layer->write(_address);
	}
	else if (!exists && write_through_no_write_allocate) {

		std::cout << "wrote to next layer in layer " << name << ", address: " << _address << std::endl;
		// we just write to the next layer because it is a no-write-allocate
		m_next_layer->write(_address);
	}
	else if (exists && !write_through_no_write_allocate){
		std::cout << "wrote to " << _address << " and set dirty bit" << std::endl;
		set_dirty_bit(address.index, address.tag);
	}
	else if (!exists && !write_through_no_write_allocate) {
		std::cout << "brought " << _address << " into cache, wrote to it, and set dirty bit" << std::endl;
		find_place(_address);
		set_dirty_bit(address.index, address.tag);
	}
}


void Cache::set_dirty_bit(int index, int tag)
{
	// select the set which the value should be
	CacheSet& cs = m_cache[index % n_sets];

	for (size_t i = 0; i != cs.entries.size(); ++i) {
		if (cs.entries[i].tag == tag) {
			cs.entries[i].dirty = 1;
		}
	}
}