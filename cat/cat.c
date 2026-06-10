#include "file.h"
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum error_code
{
  ERR_FOPEN = 1,
  ERR_FREAD = 2,
  ERR_FWRITE = 3,
  ERR_FACCESS = 4,
  ERR_FINVAL = 5,
};

int write_file_content(file_struct *file, int nbytes_read, char **error_buffer);
void write_error(char *error_message, char **error_buffer);
void copy_string(char *in, char **out, size_t in_size);

const enum error_code error;

/* Accept files as command-line input and display their content out to standard
   output. */
int main(int argc, char **argv)
{
  file_struct file;
  file.size = 0;
  file.path = NULL;
  file.content = NULL;
  char *error_buffer = NULL;

  int bytes_read;
  int bytes_written;

  for (int i = 1; i <= argc; i++)
  {
    // if no file operand is specified
    if (argc == 1)
    {
      file.fd = STDIN_FILENO;
    }
    else if (argv[i])
    {
      // separating input files by whitespace
      copy_string(argv[i], &file.path, strlen(argv[i]));

      if (strcmp(file.path, "-") == 0)
      {
        file.fd = STDIN_FILENO;
      }
      else if ((file.fd = open_file(&file, &error_buffer)) == -1)
      {
        errx(ERR_FOPEN, "%s: %s", file.path, error_buffer);
      }
      else if (!is_regular_file(&file, &error_buffer))
      {
        errx(ERR_FINVAL, "%s: %s", file.path, error_buffer);
      }
    }

    // read file content and write to stdout
    while ((bytes_read = read_file(&file, &error_buffer)) > 0)
    {
      if ((bytes_written = write_file_content(&file, bytes_read, &error_buffer)) == -1)
      {
        errx(ERR_FWRITE, "%s: %s", file.path, error_buffer);
      }
    }

    if (bytes_read == -1)
    {
      errx(ERR_FREAD, "%s: %s", file.path, error_buffer);
    }

    file.size += bytes_written;
  }

  close(file.fd);
}

/* Check if FILE is a regular file. If false, return 0 and
   write error message into ERROR_BUFFER. Otherwise return 1. */
int is_regular_file(file_struct *file, char **error_buffer)
{
  struct stat file_stat_info;
  int is_reg_file = 0;

  // retrieve file attributes using stat() and store it in struct file_stat_info
  if (stat(file->path, &file_stat_info) == -1)
  {
    write_error("File cannot be accessed", error_buffer);
    return is_reg_file;
  }

  if (S_ISREG(file_stat_info.st_mode))
  {
    is_reg_file = 1;
  }
  else if (S_ISDIR(file_stat_info.st_mode))
  {
    write_error("Is a directory", error_buffer);
  }
  else
  {
    write_error("Invalid file type", error_buffer);
  }

  return is_reg_file;
}

/* Write NBYTES_READ bytes of FILE to stdout. Upon error,
   write error message into ERROR_BUFFER and return -1.
   Otherwise, return number of bytes written. */
int write_file_content(file_struct *file, int nbytes_read, char **error_buffer)
{
  int bytes_written;

  if ((bytes_written = write(STDOUT_FILENO, file->content, nbytes_read)) == -1)
  {
    write_error("File content cannot be written", error_buffer);
  }

  return bytes_written;
}
