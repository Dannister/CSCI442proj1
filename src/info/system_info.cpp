#include "system_info.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <set>

using namespace std;


double get_uptime() {
  double uptime;
	ifstream uptime_file(PROC_ROOT "/uptime");
	if(!uptime_file){
		cerr << "Unable to read from /proc/uptime" << endl;
		return 0.0;
	}

	uptime_file >> uptime;

  return uptime;
}

SystemInfo get_system_info() {
  SystemInfo sysInfo;

  sysInfo.load_average = get_load_average();
  sysInfo.uptime = get_uptime();
  sysInfo.memory_info = get_memory_info();
  sysInfo.cpus = get_cpu_info();
  sysInfo.processes = get_all_processes(PROC_ROOT);
  sysInfo.num_processes = sysInfo.processes.size();
  sysInfo.num_threads = 0;
  sysInfo.num_running = 0;
  set<unsigned> threads;
  set<unsigned> userThreads;
  for(int i = 0; i < sysInfo.num_processes; i++){
  		sysInfo.num_threads += sysInfo.processes[i].threads.size();
  		for(int j = 0; j < sysInfo.processes[i].threads.size(); j++){
  			if(sysInfo.processes[i].threads[j].is_thread()){
  				threads.insert(sysInfo.processes[i].threads[j].pid);
  			}
  			if(sysInfo.processes[i].threads[j].is_user_thread()){
  				userThreads.insert(sysInfo.processes[i].threads[j].pid);
  			}
  			if(sysInfo.processes[i].threads[j].state == 'R'){
  				sysInfo.num_running++;
  			}
  		}
  }
  sysInfo.num_threads = threads.size();
  sysInfo.num_user_threads = userThreads.size();
  sysInfo.num_kernel_threads = sysInfo.num_threads - sysInfo.num_user_threads;

  return sysInfo;
}
