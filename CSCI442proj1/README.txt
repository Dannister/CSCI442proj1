Name: Dan Herman
Unusual/interesting features: Sometimes a process will use -2000% of the CPU.
Approximate hours spent: 25
Ideal terminal size: 800x600 pixels
List of important files:
  cpu_info: The cpu_info header/cpp files get information about the CPUs.
            This data is taken from /proc/stat and put into the structure
            defined in the header file for cpu_info.

  load_average_info: These files get information about the load average from
            /proc/loadavg and places the information into a struct as defined
            by the header file.

  memory_info: These files take information from /proc/meminfo and place them in
            the MemoryInfo struct defined in the header file memory_info.h.

  process_info: These files take information from various directories in
            /proc/[pid]: statm, stat, cmdline, comm, and status. The behavior
            is slightly different if the base directory is /proc/[pid]/task. If
            the base directory is not a task directory, the process's threads
            are set by running /proc/[pid]/task through the process_info
            get_all_processes() function.

  system_info: All of the data from each aforementioned file is conglomerated
            by this file into a SystemInfo struct. Most of it is just simply
            getting info from other files, but getting the number of
            running processes, threads, user threads, and kernel threads
            requires going through the vector of processes and counting
            which are threads, user threads, and have a state of running.

  main.cpp: THIS IS WHERE THE MAGIC HAPPENS. All of the information comes
            together here, in this file, to create the best display of
            information you've ever seen! First, it displays the uptime in the
            format of HH:MM:SS and the load average information discussed above!
            Then it displays a table of CPU information! The first row is the
            average of all CPUS on the system, showing the percentage spent in
            user mode, kernel mode, and idle. Following the first row, specific
            information for each CPU is given. After that, the wonderful
            display gives the total number of processes, the total number of
            running processes, and the total number of threads! Then it gets
            into detail about the system's memory! In units of KiB, it provides
            the total installed memory, the memory currently in-use, and the
            total available memory left to be utilized by an adventurous
            Linux user such as yourself! After that, a table of processes is
            shown! Based on the command line arguments described below, the
            table can be sorted by PID, resident memory (RSS), percentage of CPU
            utilization, and the execution time of
            the process! But wait, there's more! Included with the information
            that the table can be sorted by, there is information about the
            process's state and the command used to start the process!

  Makefile: There is a makefile that allows the user to type make and it creates
            an executable called mytop.

  mytop: This executable takes three options: -d which sets the delay in tenths
            of seconds, -s which takes PID, CPU, MEM, and TIME and sorts the
            processes by the associated column, and -h which provides pretty
            much exactly the information I just gave in this long sentence.
            mytop displays all of the information gathered in the above files.

  readme: This file discusses the project, giving valuable information such as
            my name, how many hours this project took me, the ideal terminal
            size in which to run mytop, and information about most of the files
            contained within my submission. I have omitted those that I did not
            have a role in writing and are not relevant to the final
            deliverable.


Note: quicksort algorithms adapted from:
  http://code.runnable.com/VEzwVDYBBzgHZ0VI/quicksort-for-c%2B%2B-and-sorting
