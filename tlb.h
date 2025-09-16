#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <sstream>


class PT {
public:
	// this is used to record how many initial accesses there have been because
	// the write-up says the first 1-n physical pages are allocated as 0-n before LRU is used
	int m_accessed_count = 0;

	// used for LRU calculations
	std::map<int, int> m_time_address;
	std::unordered_map<int, int> m_address_time;
	int m_timer = 0;

	// this maps the virtual address to a physical one
	std::unordered_map<int, int> m_v_to_phy;

	// lets us know if a virtual address currently points to a page that is not
	// currently in memory
	std::unordered_map<int, int> m_present;

	// uses LRU to evict and make space for a new entry
	int evict_entry();

	// this is used to make sure the data structures are managing time correctly
	void update_address_time(int address);


	int n_virtual_pages = 0, n_physical_pages = 0;
public:

	// request the translation for a virtual address
	int request(std::string _address);

	PT(std::map<std::string, int>& pt);
};


int PT::evict_entry()
{
	// get the physical address that will be freed up
	int result = -1;
	int address = m_time_address.begin()->second;
	result = m_v_to_phy[address];

	// erase the data structure entries
	m_time_address.erase(m_time_address.begin());
	m_address_time.erase(m_address_time.find(address));

	// update the address valid bit
	m_present[address] = 0;

	return result;
}

void PT::update_address_time(int address)
{
	// erase the time->address mapping
	std::unordered_map<int, int>::iterator itr = m_address_time.find(address);

	if (itr != m_address_time.end()) {
		m_time_address.erase(m_time_address.find(m_address_time[address]));
	}

	// set the entries to the new time
	m_time_address[m_timer] = address;
	m_address_time[address] = m_timer;
	
	// incrememnt the clock
	m_timer++;
}

int PT::request(std::string _address)
{
	// read in the hex address and convert it into an int
	int address;
	std::stringstream ss(_address);
	ss >> std::hex >> address;


	// check to see if it exists. if present bit is 0, either it does not exist
	// in memory or it could have not been initialized yet

	// has not been initalized yet
	if (m_present[address] == 0 && m_accessed_count < n_physical_pages) {
		std::cout << "page not present and not allocated yet" << std::endl;
		// allocate the next available physical page and increment used physical pages
		m_v_to_phy[address] = m_accessed_count;
		m_present[address] = 1;
		m_accessed_count++;

		m_time_address[m_timer] = address;
		m_address_time[address] = m_timer;
		m_timer++;
	}

	// it is just not in memory so it is on disk. We dont simulate the disk part but
	// we still need to do LRU on current physical pages
	else if (m_present[address] == 0) {
		std::cout << "page not present" << std::endl;
		// get the physical address of the evicted page
		int physical_address = evict_entry();

		// set the map
		m_v_to_phy[address] = physical_address;
		m_present[address] = 1;
		update_address_time(address);
	}

	// the mapping exists and the page is present
	else {
		update_address_time(address);
		std::cout << "page present" << std::endl;
	}


	return m_v_to_phy[address];
}


PT::PT(std::map<std::string, int>& pt)
{
	int page_size = pt["Page size"];
	n_virtual_pages = pt["Number of virtual pages"];
	n_physical_pages = pt["Number of physical pages"];
}



class TLB {
public:


public:

	TLB(std::map<std::string, int>& pt, std::map<std::string, int>& tlb);
};





TLB::TLB(std::map<std::string, int>& pt, std::map<std::string, int>& tlb)
{

}