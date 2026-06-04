# Pre-project/Research

- read POSIX spec for own implementation
- use low level syscalls from unistd.h instead of stdio.h; more hands-on
- incorporate error-handling from day one to build the habit
  - look into perror and strerror(errno)
- compile with everything on: gcc -Wall -Wextra -Wpedantic -fsanitisze=address,undefined -g
  - 'address' catches memory errors and 'undefined' catches code resulting in undefined behaviour
- use gdb for debugging; learn basics of it
- implement flags too: -n,-e,-s, etc;
  - look into `getopt()`

# Project Notes

## Implementation Decisions

### Error Handling

I could create an error struct that holds:
  1. `int error_num; /* 0 for success, -1 for error */`
  2. `char *error_message `

When a function encounters an error:
  - sets `error_num` = -1
  - writes into `error_message`
  - returns `error_num`  

if I combine this struct with relevant functions I could re-use the error handling pattern.
This is similar to `errno`, `err()`, `error()`; I'll look into them to see if i can just use them.

## General info on Cat 
- trying to change the file position on a file that doesn't support random access results in an error
- if the '-' operand is used multiple times and stdin is regular file, the output after the first is just 'null'
- used `cat` on mp4 and dmg files, seems like it prints out the raw sequence of bytes of a file
- if stdout is a regular file and is the same as any file operand, this can be treated as an error
- when taking stdin, it should only stop once ctrl+d is pressed
- the program will receive each file name as separate argv element 


# Additional learning resources

1. Advanced Programming in the Unix Environment by Richard Stevens
   - has a chapter on standard I/O and one on files and directories
2. The Linux Programming Interface by Michael Kerrisk
   - more modern and linux-specific

Don't need to read these cover to cover, just dip in when needed.
