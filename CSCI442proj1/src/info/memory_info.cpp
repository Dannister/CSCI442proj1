#include "memory_info.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;


MemoryInfo get_memory_info() {
  MemoryInfo mem_info;

  ifstream meminfo_file(PROC_ROOT "/meminfo");
	if (!meminfo_file) {
		cerr << "Unable to read from /proc/meminfo" << endl;
		return mem_info;
	}

	string line;
	while (getline(meminfo_file, line)) {
		istringstream iss(line);
		string firstWord;
		iss >> firstWord;
		if(firstWord == "MemTotal:") {
		  iss >> mem_info.total_memory;
		}
		else if(firstWord == "MemFree:") {
		  iss >> mem_info.free_memory;
		}
		else if(firstWord == "Buffers:") {
		  iss >> mem_info.buffers_memory;
		}
		else if(firstWord == "Cached:") {
		  iss >> mem_info.cached_memory;
		}
		else if(firstWord == "SwapTotal:") {
		  iss >> mem_info.total_swap;
		}
		else if(firstWord == "SwapFree:") {
		  iss >> mem_info.free_swap;
		  break;
		}
	}

	meminfo_file.close();

  return mem_info;
}
