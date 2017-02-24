/**
 * Contains the main() routine of what will eventually be your version of top.
 */

#include <cstdlib>
#include <ncurses.h>
#include <info/system_info.h>

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

  while (true) {
    wclear(stdscr);
    int row, col;
    getmaxyx(stdscr, row, col);

    SystemInfo sysInfo = get_system_info();

    // uptime
    printw("BEHOLD! THE uptime: %f\n", sysInfo.uptime);

    // load average
    printw("BEHOLD! THE load_average\n");
    printw("1 min: %f 5 min: %f 15 min: %f\n", sysInfo.load_average.one_min,
      sysInfo.load_average.five_mins, sysInfo.load_average.fifteen_mins);

    std::vector<CpuInfo> cpus = sysInfo.cpus;
    int initialRows = 3; // uptime, load average info
    int rowsCpuOccupies = cpus.size() + 3; // +3 for CPU totals and whitespace
    int rowsOtherStatsOccupy = 7; // totally hard coded, I hope I can count
    int totalRowsBeforeTable = initialRows + rowsCpuOccupies +
      rowsOtherStatsOccupy + 1;

    mvprintw(initialRows + 1, 0, "CPU 0:");

    // percent of time all processors in user mode, kernel mode, idling

    // percent of time each processor in user, kernel, idling

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
      "BEHOLD! THE installed memory: %u\n", sysInfo.memory_info.total_memory);

    // currently in use
    mvprintw(initialRows + rowsCpuOccupies + 5, 0,
      "BEHOLD! THE in-use memory: %u\n", sysInfo.memory_info.total_memory -
      sysInfo.memory_info.free_memory - sysInfo.memory_info.buffers_memory -
      sysInfo.memory_info.cached_memory);

    // currently available
    mvprintw(initialRows + rowsCpuOccupies + 6, 0,
      "BEHOLD! THE available memory: %u\n", sysInfo.memory_info.free_memory);

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
