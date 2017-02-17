#include "cpu_info.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;


vector<CpuInfo> get_cpu_info() {
	vector<CpuInfo> cpu_info;

	ifstream cpuinfo_file(PROC_ROOT "/stat");
	if(!cpuinfo_file){
		cerr << "Unable to read from /proc/stat" << endl;
		return cpu_info;
	}

	string line;
	while(getline(cpuinfo_file, line)){
		CpuInfo tmp;
		istringstream iss(line);
		string firstWord;
		iss >> firstWord;
		if(firstWord.substr(0,3) == "cpu"){
		  iss >> tmp.user_time;
		  iss >> tmp.nice_time;
		  iss >> tmp.system_time;
		  iss >> tmp.idle_time;
		  iss >> tmp.io_wait_time;
		  iss >> tmp.irq_time;
		  iss >> tmp.softirq_time;
		  iss >> tmp.steal_time;
		  iss >> tmp.guest_time;
		  iss >> tmp.guest_nice_time;
		  cpu_info.push_back(tmp);
		}
	}

	cpuinfo_file.close();

  return cpu_info;
}


CpuInfo operator -(const CpuInfo& lhs, const CpuInfo& rhs) {
	CpuInfo result;
	result.user_time = lhs.user_time - rhs.user_time;
	result.nice_time = lhs.nice_time - rhs.nice_time;
	result.system_time = lhs.system_time - rhs.system_time;
	result.idle_time = lhs.idle_time - rhs.idle_time;
	result.io_wait_time = lhs.io_wait_time - rhs.io_wait_time;
	result.irq_time = lhs.irq_time - rhs.irq_time;
	result.softirq_time = lhs.softirq_time - rhs.softirq_time;
	result.steal_time = lhs.steal_time - rhs.steal_time;
	result.guest_time = lhs.guest_time - rhs.guest_time;
	result.guest_nice_time = lhs.guest_nice_time - rhs.guest_nice_time;
  return result;
}
