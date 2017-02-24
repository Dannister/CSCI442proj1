/**
 * Contains the main() routine of what will eventually be your version of top.
 */

#include <cstdlib>
#include <ncurses.h>
#include <info/system_info.h>
#include <sstream>
#include <unistd.h>

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

string formatSecondsDDHHMMSS(int seconds) {
  int hours = seconds / 3600;
  int minutes = seconds / 60;
  int remainingSeconds = seconds % 60;
}

string formatSecondsMSSCC(int seconds) {

}




/**
 * Entry point for the program.
 */
int main() {
  // ncurses initialization
  initscr();

  // Don't show a cursor.
  curs_set(FALSE);

  // Set getch to return after 1000 milliseconds; this allows the program to
  // immediately respond to user input while not blocking indefinitely.
  timeout(1000);

  int tick = 1;

  int prevCPUTime = 0;

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
    // PID, resident memory size, current state (single letter),
    // % of CPU currently being used,
    // total time spent being executed (HH:MM:SS),
    // cmdline that was executed
    int displayNum = 10;
    if (sysInfo.processes.size() < 10) {
      displayNum = sysInfo.processes.size();
    }
    for (int i = 0; i < displayNum; i++) {
      int currRow = totalRowsBeforeTable + 1 + i;
      int pidLength = getLengthOfNum(sysInfo.processes.at(i).pid);
      mvprintw(currRow, 5 - pidLength, "%d", sysInfo.processes.at(i).pid);

      int rssLength = getLengthOfNum(sysInfo.processes.at(i).rss);
      mvprintw(currRow, 15 - rssLength, "%d", sysInfo.processes.at(i).rss);

      mvprintw(currRow, 24, "%c", sysInfo.processes.at(i).state);

      int lastProcessor = sysInfo.processes.at(i).processor;
      int processorTotalTime
      mvprintw(currRow, 31, "%1.1f%%", sysInfo.processes.at(i).cpu_percent);

      seconds = (sysInfo.processes.at(i).utime +
        sysInfo.processes.at(i).stime) /
        sysconf(_SC_CLK_TCK);
      hours = seconds / 3600;
      remainingSeconds = seconds % 3600;
      minutes = remainingSeconds / 60;
      remainingSeconds = remainingSeconds % 60;
      mvprintw(currRow, 38, "%02d:%02d:%02d", hours, minutes, remainingSeconds);

      mvprintw(currRow, 55 - cmdStr.length(),
          sysInfo.processes.at(i).command_line.c_str());
    }


    // Display the counter using printw (an ncurses function)
    printw("\n\nBehold, the number:\n%d", tick++);

    // Redraw the screen.
    refresh();

    // End the loop and exit if Q is pressed
    exit_if_user_presses_q();
  }

  // ncurses teardown
  endwin();

  return EXIT_SUCCESS;
}
