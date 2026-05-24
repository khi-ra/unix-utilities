# Pre-project/Research
- read POSIX spec for own implementation
- use low level syscalls from unistd.h instead of stdio.h; more hands-on
- incorporate error-handling from day one to build the habit
  - look into perror and strerror(errno)
- compile with everything on: gcc -Wall -Wextra -Wpedantic -fsanitisze=address,undefined -g
  - 'address' catches memory errors and 'undefined' catches code resulting in undefined behaviour
- use gdb for debugging; learn basics of it
- implement flags too: -n,-e,-s, etc;
  - look into getopt

# Project Notes 
- trying to change the file position on a file that doesn't support random access results in an ESPIPE error
- if the '-' operand is used multiple times and stdin is regular file, the output after the first is just 'null'

# Additional learning resources
1. Advanced Programming in the Unix Environment by Richard Stevens
   - has a chapter on standard I/O and one on files and directories
2. The Linux Programming Interface by Michael Kerrisk
   - more modern and linux-specific
Don't need to read these cover to cover, just dip in when needed.
