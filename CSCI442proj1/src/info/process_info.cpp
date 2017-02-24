#include "process_info.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;


ProcessInfo get_process(int pid, const char* basedir) {
  ProcessInfo process_info;

  // convert pid to a string
  ostringstream oss;
  oss << pid;
  string pidStr = oss.str();

  // make a baseDirectory string which is basedir/[pid]/
  string baseString = basedir;
  string baseDirectory = baseString + "/" + pidStr + "/";

  // read from statm
  string statmDirectory = baseDirectory + "statm";
  const char* statmCharArr = statmDirectory.c_str();

  ifstream statm_file(statmCharArr);
  if(!statm_file){
    cerr << "Unable to read from " + pidStr + "/statm" << endl;
    return process_info;
  }

  string line;
  while(getline(statm_file, line)){
    istringstream ss(line);
    ss >> process_info.size;
    ss >> process_info.resident;
    ss >> process_info.share;
    ss >> process_info.trs;
    ss >> process_info.lrs;
    ss >> process_info.drs;
    ss >> process_info.dt;
  }

  statm_file.close();

  // get info from stat
  string statDirectory = baseDirectory + "stat";
  const char* statCharArr = statDirectory.c_str();

  ifstream stat_file(statCharArr);
  if(!stat_file){
    cerr << "Unable to read from " + pidStr + "/stat" << endl;
    return process_info;
  }

  stat_file
     >> process_info.pid
     >> process_info.comm
     >> process_info.state
     >> process_info.ppid
     >> process_info.pgrp
     >> process_info.session
     >> process_info.tty_nr
     >> process_info.tpgid
     >> process_info.flags
     >> process_info.minflt
     >> process_info.cminflt
     >> process_info.majflt
     >> process_info.cmajflt
     >> process_info.utime
     >> process_info.stime
     >> process_info.cutime
     >> process_info.cstime
     >> process_info.priority
     >> process_info.nice
     >> process_info.num_threads
     >> process_info.itrealvalue
     >> process_info.starttime
     >> process_info.vsize
     >> process_info.rss
     >> process_info.rsslim
     >> process_info.startcode
     >> process_info.endcode
     >> process_info.startstack
     >> process_info.kstkesp
     >> process_info.kstkeip
     >> process_info.signal
     >> process_info.blocked
     >> process_info.sigignore
     >> process_info.sigcatch
     >> process_info.wchan
     >> process_info.nswap
     >> process_info.cnswap
     >> process_info.exit_signal
     >> process_info.processor
     >> process_info.rt_priority
     >> process_info.policy
     >> process_info.delayacct_blkio_ticks
     >> process_info.guest_time
     >> process_info.cguest_time;

  stat_file.close();

  // read from cmdline
  string cmdlineDirectory = baseDirectory + "cmdline";
  const char* cmdlineCharArr = cmdlineDirectory.c_str();

  ifstream cmdline_file(cmdlineCharArr);
  if(!cmdline_file){
    cerr << "Unable to read from " + pidStr + "/cmdline" << endl;
    return process_info;
  }

  string tmp = "";
  cmdline_file >> tmp;

  replace(tmp.begin(), tmp.end(), '\0', ' ');
  tmp = tmp.substr(0, tmp.length() - 1);
  cmdline_file.close();

  // check if tmp is empty and update from comm if necessary
  if(tmp == ""){
    string commDirectory = baseDirectory + "comm";
	const char* commCharArr = commDirectory.c_str();
    ifstream comm_file(commCharArr);
    if(!comm_file){
      cerr << "Unable to read from " + pidStr + "/comm";
      return process_info;
    }
    comm_file >> tmp;
    comm_file.close();
  }

  process_info.command_line = tmp;

  // check if basedir is a /task directory
  if(baseString.substr(baseString.length() - 4,4) != "task"){
    string taskDir = baseDirectory + "task";
	const char* taskArr = taskDir.c_str();
    process_info.threads = get_all_processes(taskArr);
  }

  // read from status
  string statusDirectory = baseDirectory + "status";
  const char* statusArr = statusDirectory.c_str();
  ifstream status_file(statusArr);
  if(!status_file){
    cerr << "Unable to read from" + pidStr + "/status" << endl;
    return process_info;
  }

  while (getline(status_file, line)) {
    string firstWord;
    status_file >> firstWord;
    if(firstWord == "Tgid:") {
	  status_file >> process_info.tgid;
	}
  }

  status_file.close();


  return process_info;
}


vector<ProcessInfo> get_all_processes(const char* basedir) {
  vector<ProcessInfo> proccesses;

  DIR *directory = NULL;
  struct dirent *dirEnt = NULL;
  directory = opendir(basedir);
  if (directory != NULL){
    while (dirEnt = readdir(directory)){
      if (dirEnt == NULL) {
          cerr << "Error setting up dirEnt\n";
          exit (1);
      }
      string entry = dirEnt->d_name;
      if(isdigit(entry[0])){
        int pidInt = 0;
        stringstream ss(entry);
        ss >> pidInt;
        ProcessInfo tmp = get_process(pidInt, basedir);
        proccesses.push_back(tmp);
      }
    }
  }

  closedir(directory);

  return proccesses;
}
