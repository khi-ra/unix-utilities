#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXINPUT 1024
#define MAXERROR 256

ssize_t read_input(char *input_buffer);
int read_file(char *path, char *error_buffer);
void copy_string(char *in, char *out);

/* copies contents of IN into OUT */
void copy_string(char *in, char *out)
{
  int i = 0;
  while (*(in + i) != '\0')
    *(out + i) = *(in + i);
}

/* Read input from stdin and store it in INPUT_BUFFER.
   Return the number of bytes read or -1 for error. */
ssize_t read_input(char *input_buffer)
{
  ssize_t n = -1;

  // read MAXINPUT chars from stdin and store in buffer
  if ((n = read(STDIN_FILENO, input_buffer, MAXINPUT)) == -1)
  {
    perror("read failed");
    return n;
  }

  // strip trailing '\n' and null terminate buffer
  input_buffer[strcspn(input_buffer, "\n")] = 0;
  return n;
}

int read_file(char *path, char *error_buffer)
{
  char file_data[MAXINPUT];
  struct stat file_info;
  int fd;
  int nbytes_read;
  int absolute_path = path[0] == '/';

  /* Implement Custom Errors in read_file():
     read_file() writes error messages into an 'error buffer' passed in by main.
     If read_file() returns -1 for error, the caller should check this buffer
     for specific information about the error. */

  /* refactor file opening logic into open_file() method */
  /* refactor regular-file-check into is_regular_file() method */
  if (absolute_path)
  {
    if ((fd = open(path, O_RDONLY)) == -1 || stat(path, &file_info) == -1)
    { // handle file opening/checking error
      copy_string("file-open/check error", error_buffer);
      return -1;
    }
    else if (S_ISDIR(file_info.st_mode))
    { // if its a directory, throw custom error
      copy_string("directory error", error_buffer);
      return -1;
    }
    else if (S_ISREG(file_info.st_mode))
    { // if it's a regular file, read file-stream into file_data buffer
      if ((nbytes_read = read(fd, file_data, MAXINPUT)) == -1)
      {
        copy_string("file-read error", error_buffer);
      }
      return nbytes_read;
    }
  }
  else
  { // if relative path, get working dir's fd and pass into openat()
    int working_dir_fd = open("./", O_RDONLY);
    fd = openat(working_dir_fd, path, O_RDONLY);
  }
  return fd;
}

int main(int argc, char **argv)
{
  // +1 for null termination character
  char input_buffer[MAXINPUT + 1];
  char *error_buffer = malloc(MAXERROR + 1);
  int n;
  int fd;

  if ((n = read_input(input_buffer)) > 0 &&
      (fd = read_file(input_buffer, error_buffer)) > 0)
  {
    printf("path: %s, fd: %d\n", input_buffer, n);
  }

  free(error_buffer);
}
