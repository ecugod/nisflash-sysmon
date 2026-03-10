# NISFLASH SysMon

Real-time system monitoring dashboard written in C using ncurses.

## Features

- **CPU Usage Monitoring**: Real-time CPU usage percentage with visual progress bar
- **Memory Tracking**: Memory usage with detailed breakdown (total, used, free)
- **System Uptime**: Display how long the system has been running
- **Load Average**: 1, 5, and 15 minute load averages
- **Color-coded Status**: Green (good), Yellow (warning), Red (critical)

## Building

```bash
make
```

## Running

```bash
./sysmon
```

Press 'q' to quit.

## Dependencies

- ncurses library (`libncurses-dev` on Debian/Ubuntu)
- Linux /proc filesystem

## Building from Source

```bash
gcc -o sysmon sysmon.c -lncurses
```

## Usage

Just run the binary:
```
./sysmon
```

The dashboard updates every second. Press 'q' to exit.

---

**NISFLASH** - Built for performance.
