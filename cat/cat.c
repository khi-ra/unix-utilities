#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXFILEDATA 1024
#define MAXINPUT 256
#define MAXERROR 256

enum error_code
{
  ERR_FOPEN = 1,
  ERR_FREAD = 2,
  ERR_FWRITE = 3,
  ERR_FACCESS = 4,
  ERR_FINVAL = 5,
};

typedef struct
{
  int fd;
  char content[MAXFILEDATA + 1];
  char path[MAXINPUT + 1];
  size_t size;
} file_struct;

int open_file(file_struct *file, char *error_buffer);
int is_regular_file(file_struct *file, char *error_buffer);
int read_file(file_struct *file, char *error_buffer);
int write_file_content(file_struct *file, int bytes_read, char *error_buffer);
void copy_string(char *in, char *out, size_t in_size);

/* Custom error messages: Any function that takes ERROR_BUFFER as an arg writes
   an error message into it. If said function returns -1, the caller should
   check this buffer for specific information about the error. */

const enum error_code error;

/* Accept files as command-line input and display their content out to standard
   output. */
int main(int argc, char **argv)
{
  file_struct file;
  file.size = 0;

  int read_bytes;
  int write_bytes;

  char error_buffer[MAXERROR + 1];

  for (int i = 1; i <= argc; i++)
  {
    if (!argv[i] && i == 1)
    {
      file.fd = STDIN_FILENO;
    }
    else if (argv[i])
    {
      int path_length = strcspn(argv[i], " ");
      copy_string(argv[i], file.path, path_length);

      if (strcmp(file.path, "-") == 0)
      {
        file.fd = STDIN_FILENO;
      }
      else if ((file.fd = open_file(&file, error_buffer)) == -1)
      {
        errx(ERR_FOPEN, "%s: %s", file.path, error_buffer);
      }
      else if (!is_regular_file(&file, error_buffer))
      {
        errx(ERR_FINVAL, "%s: %s", file.path, error_buffer);
      }
    }

    // read file content and write to stdout
    while ((read_bytes = read_file(&file, error_buffer)) > 0)
    {
      if ((write_bytes = write_file_content(&file, read_bytes, error_buffer)) == -1)
        errx(ERR_FWRITE, "%s: %s", file.path, error_buffer);
    }

    if (read_bytes == -1)
      errx(ERR_FREAD, "%s: %s", file.path, error_buffer);
  }

  close(file.fd);
}

/* Open FILE in read-only mode and return it's file descriptor, or -1 for
   error. */
int open_file(file_struct *file, char *error_buffer)
{
  int dir_fd = open("./", O_RDONLY);

  // if path is absolute, 'dir_fd' is ignored and only the path is used
  if (dir_fd == -1 || (file->fd = openat(dir_fd, file->path, O_RDONLY)) == -1)
  {
    copy_string("File cannot be opened", error_buffer, sizeof("File cannot be opened"));
    return -1;
  }

  close(dir_fd);
  return file->fd;
}

/* Check if FILE is a regular file. If false, return 0 and
   write error message into ERROR_BUFFER. Otherwise return 1. */
int is_regular_file(file_struct *file, char *error_buffer)
{
  struct stat file_stat_info;
  int is_reg_file = 0;

  // retrieve file attributes using stat() and store it in struct file_stat_info
  if (stat(file->path, &file_stat_info) == -1)
  {
    copy_string("File cannot be accessed", error_buffer, sizeof("File cannot be accessed"));
    return is_reg_file;
  }

  if (S_ISREG(file_stat_info.st_mode))
  {
    is_reg_file = 1;
  }
  else if (S_ISDIR(file_stat_info.st_mode))
  {
    copy_string("Is a directory", error_buffer, sizeof("Is a directory"));
  }
  else
  {
    copy_string("Invalid file type", error_buffer, sizeof("Invalid file type"));
  }

  return is_reg_file;
}

/* Read file and store it's content into FILE.content. Upon
   error, write error message into ERROR_BUFFER and return -1.
   Otherwise, return number of bytes read. */
int read_file(file_struct *file, char *error_buffer)
{
  int nbytes;

  if ((nbytes = read(file->fd, file->content, MAXFILEDATA)) == -1)
  {
    copy_string("File cannot be read", error_buffer, sizeof("File cannot be read"));
    return -1;
  }
  file->content[nbytes] = 0;

  return nbytes;
}

/* Write BYTES_READ bytes of FILE to stdout. Upon error,
   write error message into ERROR_BUFFER and return -1.
   Otherwise, return number of bytes written.*/
int write_file_content(file_struct *file, int bytes_read, char *error_buffer)
{
  int write_bytes;

  if ((write_bytes = write(STDOUT_FILENO, file->content, bytes_read)) == -1)
  {
    copy_string("File content cannot be written", error_buffer, sizeof("File content cannot be written"));
    return -1;
  }

  return write_bytes;
}

/* Copy contents of IN to OUT */
void copy_string(char *in, char *out, size_t in_size)
{
  memmove(out, in, in_size);
  *(out + in_size) = 0;
}
