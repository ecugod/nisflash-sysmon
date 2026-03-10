#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <time.h>

#define REFRESH_RATE 1

void draw_header() {
    attron(A_BOLD | A_REVERSE);
    mvprintw(0, 0, " %-20s", "NISFLASH SysMon");
    mvprintw(0, 40, " %-20s", "Real-Time System Monitor");
    attroff(A_BOLD | A_REVERSE);
}

void draw_cpu_info() {
    FILE *stat = fopen("/proc/stat", "r");
    static unsigned long long prev_idle = 0, prev_total = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long long total, curr_idle, curr_total, diff_idle, diff_total;
    double cpu_usage;

    if (!stat) return;

    fscanf(stat, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
    fclose(stat);

    total = user + nice + system + idle + iowait + irq + softirq + steal;
    curr_idle = idle + iowait;
    curr_total = total;

    if (prev_total != 0) {
        diff_idle = curr_idle - prev_idle;
        diff_total = curr_total - prev_total;
        cpu_usage = 100.0 * (1.0 - (double)diff_idle / diff_total);

        attron(A_BOLD);
        mvprintw(2, 2, "CPU Usage:");
        attroff(A_BOLD);

        if (cpu_usage < 50) {
            attron(COLOR_PAIR(1));
        } else if (cpu_usage < 80) {
            attron(COLOR_PAIR(2));
        } else {
            attron(COLOR_PAIR(3));
        }
        mvprintw(2, 13, "%5.1f%%", cpu_usage);
        attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));

        // Progress bar
        mvprintw(3, 2, "[");
        int bars = (int)(cpu_usage / 2);
        for (int i = 0; i < 50; i++) {
            if (i < bars) {
                if (cpu_usage < 50) {
                    attron(COLOR_PAIR(1));
                } else if (cpu_usage < 80) {
                    attron(COLOR_PAIR(2));
                } else {
                    attron(COLOR_PAIR(3));
                }
                printw("|");
                attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
            } else {
                printw(" ");
            }
        }
        printw("]");
    }

    prev_idle = curr_idle;
    prev_total = curr_total;
}

void draw_memory_info() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) return;

    unsigned long total = info.totalram * info.mem_unit;
    unsigned long free = info.freeram * info.mem_unit;
    unsigned long used = total - free;
    double mem_usage = 100.0 * used / total;

    attron(A_BOLD);
    mvprintw(5, 2, "Memory:");
    attroff(A_BOLD);

    if (mem_usage < 50) {
        attron(COLOR_PAIR(1));
    } else if (mem_usage < 80) {
        attron(COLOR_PAIR(2));
    } else {
        attron(COLOR_PAIR(3));
    }
    mvprintw(5, 13, "%5.1f%%", mem_usage);
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));

    // Progress bar
    mvprintw(6, 2, "[");
    int bars = (int)(mem_usage / 2);
    for (int i = 0; i < 50; i++) {
        if (i < bars) {
            if (mem_usage < 50) {
                attron(COLOR_PAIR(1));
            } else if (mem_usage < 80) {
                attron(COLOR_PAIR(2));
            } else {
                attron(COLOR_PAIR(3));
            }
            printw("|");
            attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
        } else {
            printw(" ");
        }
    }
    printw("]");

    // Detailed info
    unsigned long total_mb = total / (1024 * 1024);
    unsigned long used_mb = used / (1024 * 1024);
    mvprintw(7, 2, "Total: %lu MB | Used: %lu MB | Free: %lu MB",
             total_mb, used_mb, free / (1024 * 1024));
}

void draw_uptime() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) return;

    long uptime = info.uptime;
    long days = uptime / 86400;
    long hours = (uptime % 86400) / 3600;
    long minutes = (uptime % 3600) / 60;
    long seconds = uptime % 60;

    attron(A_BOLD);
    mvprintw(9, 2, "Uptime:");
    attroff(A_BOLD);

    mvprintw(9, 13, "%ldd %ldh %ldm %lds", days, hours, minutes, seconds);
}

void draw_processes() {
    FILE *loadavg = fopen("/proc/loadavg", "r");
    if (!loadavg) return;

    double load1, load5, load15;
    fscanf(loadavg, "%lf %lf %lf", &load1, &load5, &load15);
    fclose(loadavg);

    attron(A_BOLD);
    mvprintw(11, 2, "Load Average:");
    attroff(A_BOLD);

    mvprintw(11, 18, "%0.2f (1m)", load1);
    mvprintw(12, 18, "%0.2f (5m)", load5);
    mvprintw(13, 18, "%0.2f (15m)", load15);
}

void draw_timestamp() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    mvprintw(15, 2, "Last Updated: %s", time_str);
}

void draw_footer() {
    attron(A_DIM);
    mvprintw(LINES - 1, 0, " Press 'q' to quit");
    attroff(A_DIM);
}

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    timeout(REFRESH_RATE * 1000);

    if (!has_colors()) {
        endwin();
        printf("Your terminal doesn't support colors. Exiting.\n");
        return 1;
    }

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Good
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Warning
    init_pair(3, COLOR_RED, COLOR_BLACK);     // Critical

    int ch;
    while ((ch = getch()) != 'q') {
        erase();

        draw_header();
        draw_cpu_info();
        draw_memory_info();
        draw_uptime();
        draw_processes();
        draw_timestamp();
        draw_footer();

        refresh();
    }

    endwin();
    return 0;
}
