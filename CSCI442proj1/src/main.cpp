/**
 * Contains the main() routine of what will eventually be your version of top.
 */

#include <cstdlib>
#include <ncurses.h>
#include <info/system_info.h>
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <iostream>

using namespace std;


/**
 * Gets a character from the user, waiting for however many milliseconds that
 * were passed to timeout() below. If the letter entered is q, this method will
 * exit the program.
 */
void exit_if_user_presses_q() {
  char c = getch();

  if (c == 'q') {
    endwin();
    exit(EXIT_SUCCESS);
  }
}

int getLengthOfNum(int num) {
  stringstream ss;
  ss << num;
  string numStr = ss.str();

  return numStr.length();
}

void checkSortArg(char* argChar) {
  string arg(argChar);
  if (arg == "" || ( arg != "PID" && arg != "CPU" && arg != "MEM" &&
    arg != "TIME")) {
      std::cout << "Invalid argument to sort-key, run mytop with -h or --help."
      << "\n\n";
      exit(0);
    }
}

int partitionCPU(vector<ProcessInfo>& processes, int left, int right, double cpuPerc) {
  for (int i=left; i<right; ++i) {
    if (processes[i].cpu_percent > cpuPerc) {
      swap(processes[i], processes[left]);
      left ++;
    }
  }
  return left - 1;
}

void qsortCPU(vector<ProcessInfo>& processes, int left, int right) {
  if (left >= right) return;

  int middle = left + (right - left) / 2;
  swap(processes[middle], processes[left]);
  int midpoint = partitionCPU(processes, left + 1, right, processes[left].cpu_percent);
  swap(processes[left], processes[midpoint]);
  qsortCPU(processes, left, midpoint);
  qsortCPU(processes, midpoint + 1, right);
}

int partitionMEM(vector<ProcessInfo>& processes, int left, int right, long rss) {
  for (int i=left; i<right; ++i) {
    if (processes[i].rss > rss) {
      swap(processes[i], processes[left]);
      left ++;
    }
  }
  return left - 1;
}

void qsortMEM(vector<ProcessInfo>& processes, int left, int right) {
  if (left >= right) return;

  int middle = left + (right - left) / 2;
  swap(processes[middle], processes[left]);
  int midpoint = partitionMEM(processes, left + 1, right, processes[left].rss);
  swap(processes[left], processes[midpoint]);
  qsortMEM(processes, left, midpoint);
  qsortMEM(processes, midpoint + 1, right);
}

int partitionTIME(vector<ProcessInfo>& processes, int left, int right, long time) {
  for (int i=left; i<right; ++i) {
    if (processes[i].utime + processes[i].stime > time) {
      swap(processes[i], processes[left]);
      left ++;
    }
  }
  return left - 1;
}

void qsortTIME(vector<ProcessInfo>& processes, int left, int right) {
  if (left >= right) return;

  int middle = left + (right - left) / 2;
  swap(processes[middle], processes[left]);
  int midpoint = partitionTIME(processes, left + 1, right,
    (processes[left].utime + processes[left].stime));
  swap(processes[left], processes[midpoint]);
  qsortTIME(processes, left, midpoint);
  qsortTIME(processes, midpoint + 1, right);
}

void sortByCPU(vector<ProcessInfo> &processes) {
  qsortCPU(processes, 0, processes.size());
  return;
}

void sortByMEM(vector<ProcessInfo> &processes) {
  qsortMEM(processes, 0, processes.size());
  return;
}

void sortByTIME(vector<ProcessInfo> &processes) {
  qsortTIME(processes, 0, processes.size());
  return;
}

/**
 * Entry point for the program.
 */
int main(int argc, char **argv) {
  int delay = 3000;
  char* colToSortBy = "PID";
  int showHelpFlag = 0;
  int c;

  while(true) {
    static struct option long_options[] = {
      {"delay", required_argument, 0, 'd'},
      {"sort-key", required_argument, 0, 's'},
      {"help", no_argument, &showHelpFlag, 1},
      {0, 0, 0, 0}
    };

    int optIndex = 0;
    c = getopt_long(argc, argv, "d:s:h", long_options, &optIndex);
    if (c == -1) {
      break;
    }

    switch(c) {
      case 0:
        break;
      case 'd':
        delay = atoi(optarg) * 100;
        break;
      case 's':
        colToSortBy = optarg;
        checkSortArg(colToSortBy);
        break;
      case 'h':
        showHelpFlag = 1;
        break;
      default:
        std::cout << "Invalid option, invoke mytop with -h or --help, because "
          << "you clearly need it.\n";
        abort();
    }
  }

  if(showHelpFlag) {
    std::cout << "mytop is a simple reproduction of the top command."
      << "\nValid options are:\n\n"
      << "-d --delay DELAY - takes an integer, sets the refresh delay in "
      << "tenths of seconds.\n\n"
      << "-s --sort-key KEY - takes PID, CPU, MEM, or TIME; sets the column to "
      << "sort processes by.\n\n"
      << "-h --help - considering you just used it, you should know what it "
      << "does.\n\n";
    exit(0);
  }

  // ncurses initialization
  initscr();

  // Don't show a cursor.
  curs_set(FALSE);

  // Set getch to return after 1000 milliseconds; this allows the program to
  // immediately respond to user input while not blocking indefinitely.
  timeout(delay);

  int prevCPUTime = 0;

  std::vector<CpuInfo> oldCPUs;
  int oldCPUTotalTime = 0;
  std::vector<ProcessInfo> oldProcesses;

  while (true) {
    wclear(stdscr);
    int row, col;
    getmaxyx(stdscr, row, col);

    SystemInfo sysInfo = get_system_info();

    // uptime
    int seconds = sysInfo.uptime;
    int hours = seconds / 3600;
    int remainingSeconds = seconds % 3600;
    int minutes = remainingSeconds / 60;
    remainingSeconds = remainingSeconds % 60;
    printw("BEHOLD! THE uptime: %02d:%02d:%02d\n", hours, minutes,
      remainingSeconds);

    // load average
    printw("BEHOLD! THE load_average\n");
    printw("1 min: %1.2f 5 min: %1.2f 15 min: %1.2f\n", sysInfo.load_average.one_min,
      sysInfo.load_average.five_mins, sysInfo.load_average.fifteen_mins);

    std::vector<CpuInfo> cpus = sysInfo.cpus;
    int initialRows = 3; // uptime, load average info
    int rowsCpuOccupies = cpus.size() + 1; // +3 for CPU totals and whitespace
    int rowsOtherStatsOccupy = 7; // totally hard coded, I hope I can count
    int totalRowsBeforeTable = initialRows + rowsCpuOccupies +
      rowsOtherStatsOccupy + 2;

    int currCPUTime = cpus.at(0).total_time();

    if (prevCPUTime == 0) {
      prevCPUTime = currCPUTime;
    }

    // percent of time processors in user mode, kernel mode, idling
    for (int i = 0; i < cpus.size(); i++) {
      int currRow = initialRows + 1 + i;
      if (i == 0) {
        mvprintw(currRow, 0, "CPUS:");
      } else {
        mvprintw(currRow, 0, "CPU%d:", i-1);
      }
      float percUserTime = (float)cpus.at(i).user_time * 100 / cpus.at(i).total_time();
      mvprintw(currRow, 20 - 9, "%3.2f%% user", percUserTime);
      float percSysTime = (float)cpus.at(i).system_time * 100 / cpus.at(i).total_time();
      mvprintw(currRow, 40 - 11, "%3.2f%% system", percSysTime);
      float percIdleTime = (float)cpus.at(i).idle_time * 100 / cpus.at(i).total_time();
      mvprintw(currRow, 60 - 11, "%3.2f%% idle", percIdleTime);
    }

    // num_processes
    mvprintw(initialRows + rowsCpuOccupies + 1, 0,
      "BEHOLD! THE num_processes: %u\n", sysInfo.num_processes);

    // num processes running
    mvprintw(initialRows + rowsCpuOccupies + 2, 0,
      "BEHOLD! THE num_running: %u\n", sysInfo.num_running);

    // number of threads (user and kernel)
    mvprintw(initialRows + rowsCpuOccupies + 3, 0,
      "BEHOLD! THE num_threads: %u\n", sysInfo.num_threads);

    // amount of memory installed
    mvprintw(initialRows + rowsCpuOccupies + 4, 0,
      "BEHOLD! THE installed memory (KiB): %u\n", sysInfo.memory_info.total_memory);

    // currently in use
    mvprintw(initialRows + rowsCpuOccupies + 5, 0,
      "BEHOLD! THE in-use memory (KiB): %u\n", sysInfo.memory_info.total_memory -
      sysInfo.memory_info.free_memory);

    // currently available
    mvprintw(initialRows + rowsCpuOccupies + 6, 0,
      "BEHOLD! THE available memory (KiB): %u\n", sysInfo.memory_info.free_memory);

    // table with labeled columns:
    mvprintw(initialRows + rowsCpuOccupies + 7, 0, "BEHOLD! THE table\n");
    string pidStr = "PID";
    string rssStr = "RSS";
    string stateStr = "STATE";
    string percCPU = "%%CPU";
    string timeStr = "TIME";
    string cmdStr = "COMMAND";
    // PID column starts at col 0 and goes to col 4
    mvprintw(totalRowsBeforeTable, 5 - pidStr.length(),"PID");
    // col 5 to 14
    mvprintw(totalRowsBeforeTable, 15 - rssStr.length(), "RSS");
    // 15 to 24
    mvprintw(totalRowsBeforeTable, 25 - stateStr.length(), "STATE");
    // 25 to 34
    mvprintw(totalRowsBeforeTable, 35 - percCPU.length() + 1, percCPU.c_str());
    // 35 to 44
    mvprintw(totalRowsBeforeTable, 45 - timeStr.length(), "TIME");
    // 45 to wherever
    mvprintw(totalRowsBeforeTable, 55 - cmdStr.length(), "COMMAND");

    vector<ProcessInfo> processes = sysInfo.processes;

    string sortColumn(colToSortBy);
    if (sortColumn == "CPU") {
      sortByCPU(processes);
      //sortByCPU(oldProcesses);
    } else if (sortColumn == "MEM") {
      sortByMEM(processes);
    } else if (sortColumn == "TIME") {
      sortByTIME(processes);
    }

    // PID, resident memory size, current state (single letter),
    // % of CPU currently being used,
    // total time spent being executed (HH:MM:SS),
    // cmdline that was executed
    int displayNum = 10;
    if (processes.size() < 10) {
      displayNum = processes.size();
    }
    for (int i = 0; i < displayNum; i++) {
      int currRow = totalRowsBeforeTable + 1 + i;
      int pidLength = getLengthOfNum(processes.at(i).pid);
      mvprintw(currRow, 5 - pidLength, "%d", processes.at(i).pid);

      int rssLength = getLengthOfNum(processes.at(i).rss * 4);
      mvprintw(currRow, 15 - rssLength, "%d", processes.at(i).rss * 4);

      mvprintw(currRow, 24, "%c", processes.at(i).state);

      int elapsedTime = 4;
      int elapsedProcessTime = 1;
      if (oldCPUTotalTime != 0) {
        elapsedTime = cpus.at(0).total_time() - oldCPUTotalTime;
        mvprintw(38, 38, "Elapsed time: %d", cpus.at(0).total_time() -
          oldCPUTotalTime);
        mvprintw(39 + i, 38, "Process time: %d",
          (processes.at(i).utime - oldProcesses.at(i).utime) +
          (processes.at(i).stime - oldProcesses.at(i).stime));
        elapsedProcessTime =
          (processes.at(i).utime - oldProcesses.at(i).utime) +
          (processes.at(i).stime - oldProcesses.at(i).stime);
        processes.at(i).cpu_percent = (double)elapsedProcessTime *
          1000.0 / elapsedTime;
      }
      mvprintw(currRow, 30, "%1.1f%%", processes.at(i).cpu_percent);
      mvprintw(50, 38, "test %d %s", delay, colToSortBy);

      seconds = (processes.at(i).utime +
        processes.at(i).stime) /
        sysconf(_SC_CLK_TCK);
      hours = seconds / 3600;
      remainingSeconds = seconds % 3600;
      minutes = remainingSeconds / 60;
      remainingSeconds = remainingSeconds % 60;
      mvprintw(currRow, 38, "%02d:%02d:%02d", hours, minutes, remainingSeconds);

      mvprintw(currRow, 55 - cmdStr.length(),
          processes.at(i).comm);
    }

    oldCPUTotalTime = cpus.at(0).total_time();
    oldProcesses = processes;
    // Redraw the screen.
    refresh();

    // End the loop and exit if Q is pressed
    exit_if_user_presses_q();
  }

  // ncurses teardown
  endwin();

  return EXIT_SUCCESS;
}
