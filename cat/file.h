#ifndef FILE_H
#define FILE_H

#include <unistd.h>

#define MAXFILEDATA 4096
#define MAXINPUT 1024
#define MAXERROR 1024

extern struct file_struct file;

/* Open FILE in read-only mode and return it's file descriptor, or -1 for
   error. */
int open_file(file_struct *file, char **error_buffer);

/* Check if FILE is a regular file. If false, return 0 and
   write error message into ERROR_BUFFER. Otherwise return 1. */
int is_regular_file(file_struct *file, char **error_buffer);

/* Read file and store it's content into FILE.content. Upon
   error, write error message into ERROR_BUFFER and return -1.
   Otherwise, return number of bytes read. */
int read_file(file_struct *file, char **error_buffer);

#endif
