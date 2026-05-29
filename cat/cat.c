#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXFILEDATA 1024
#define MAXINPUT 256
#define MAXERROR 256

ssize_t read_input(char *input_buffer);
int read_file(char *path, char *file_data, char *error_buffer);
void copy_string(char *in, char *out);

/* copies contents of IN into OUT */
void copy_string(char *in, char *out)
{
  int i = 0;
  while (*(in + i) != '\0') {
    *(out + i) = *(in + i);
    i++;
  }
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

int read_file(char *path, char *file_data, char *error_buffer)
{
  struct stat file_info;
  int fd;
  int nbytes_read;
  int is_absolute_path = path[0] == '/';

  /* Custom Errors in read_file():
     read_file() writes error messages into an 'error buffer' passed in by
     main(). If read_file() returns -1 (to indicate error), main() shall check
     this buffer for specific information about the error. */

  /* refactor file opening logic into open_file() method */
  /* refactor regular-file check logic into is_regular_file() method */
  if (is_absolute_path)
  {
    if ((fd = open(path, O_RDONLY)) == -1)
    { // handle file opening error
      copy_string("file cannot be opened", error_buffer);
      return -1;
    }
    else if (stat(path, &file_info) == -1)
    { // handle file checking error
      copy_string("file cannot be accessed", error_buffer);
      return -1;
    }
    else if (S_ISDIR(file_info.st_mode))
    { // handle directory as input error
      copy_string("is a directory", error_buffer);
      return -1;
    }
    else if (S_ISREG(file_info.st_mode))
    { // if it's a regular file, read file-stream into file_data buffer
      if ((nbytes_read = read(fd, file_data, MAXFILEDATA)) == -1)
      {
        copy_string("file cannot be read", error_buffer);
        return -1;
      }
      return nbytes_read;
    }
    else
    {
      copy_string("invalid file type", error_buffer);
      return -1;
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
  char file_data[MAXFILEDATA];
  int input_size;
  int file_size;

  if ((input_size = read_input(input_buffer)) > 0 &&
      (file_size = read_file(input_buffer, file_data, error_buffer)) > 0)
  {
    printf("input path: %s\n-----------\n", input_buffer);
    printf("file size: %d\n-----------\n", file_size);
    printf("file data: %s\n", file_data);
  }

  free(error_buffer);
}
