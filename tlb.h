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
	int m_timer = 0;

	// this maps the virtual address to a physical one
	std::unordered_map<int, int> m_v_to_phy;

	// lets us know if a virtual address currently points to a page that is not
	// currently in memory
	std::unordered_map<int, int> m_present;

	int n_virtual_pages = 0, n_physical_pages = 0;
public:

	// request the translation for a virtual address
	int request(std::string _address);

	PT(std::map<std::string, int>& pt);
};




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

		// allocate the next available physical page and increment used physical pages
		m_v_to_phy[address] = n_physical_pages;
		n_physical_pages++;
	}

	// it is just not in memory so it is on disk. We dont simulate the disk part but
	// we still need to do LRU on current physical pages
	else if (m_present[address] == 0) {

	}
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