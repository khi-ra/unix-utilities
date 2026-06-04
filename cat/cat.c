#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXFILEDATA 1024
#define MAXINPUT 256
#define MAXERROR 256

struct file_struct
{
  int fd;
  // +1 for null termination character
  char content[MAXFILEDATA + 1];
  char path[MAXINPUT + 1];
  size_t size;
};

int open_file(struct file_struct *file, char *error_buffer);
int is_regular_file(struct file_struct *file, char *error_buffer);
int read_file(struct file_struct *file, char *error_buffer);
int write_from_file(struct file_struct *file, int bytes_read,
                    char *error_buffer);
void copy_string(char *in, char *out, size_t in_size);

/* Custom error messages: Any function that takes ERROR_BUFFER as an arg writes
   an error message into it. If said function returns -1, the caller should
   check this buffer for specific information about the error. */

/* Accept files as command-line input and display their content out to standard
   output. */
int main(int argc, char **argv)
{
  struct file_struct file;
  int read_bytes;
  int write_bytes;
  int i = 1;
  int arg_length;

  // +1 for null termination
  char error_buffer[MAXERROR + 1];
  file.size = 0;

  if (argc == 1)
  {
    file.fd = STDIN_FILENO;

    while ((read_bytes = read_file(&file, error_buffer)) > 0)
    {
      if ((write_bytes = write_from_file(&file, read_bytes, error_buffer)) ==
          -1)
      {
        printf("Error: %s\n", error_buffer);
        return -1;
      }
    }

    if (read_bytes == -1)
    {
      printf("Error: %s\n", error_buffer);
      return -1;
    }

    return 0;
  }

  while (argv[i] && i < argc)
  {
    arg_length = strcspn(argv[i], " ");
    copy_string(argv[i], file.path, arg_length);

    if ((file.fd = open_file(&file, error_buffer)) == -1)
    {
      printf("Error: %s\n", error_buffer);
      return -1;
    }
    else if (!is_regular_file(&file, error_buffer))
    {
      printf("Error: %s\n", error_buffer);
      return -1;
    }

    while ((read_bytes = read_file(&file, error_buffer)) > 0)
    {
      if ((write_bytes = write_from_file(&file, read_bytes, error_buffer)) ==
          -1)
      {
        printf("Error: %s\n", error_buffer);
        return -1;
      }
    }

    if (read_bytes == -1)
    {
      printf("Error: %s\n", error_buffer);
      return -1;
    }

    i++;
  }

  close(file.fd);
}

/* Open FILE in read-only mode and return it's file descriptor, or -1 for
   error. */
int open_file(struct file_struct *file, char *error_buffer)
{
  int dir_fd = open("./", O_RDONLY);

  // if path is absolute, 'dir_fd' is ignored and only the path is used
  if (dir_fd == -1 || (file->fd = openat(dir_fd, file->path, O_RDONLY)) == -1)
  {
    copy_string("File cannot be opened", error_buffer,
                sizeof("File cannot be opened"));
    return -1;
  }

  close(dir_fd);
  return file->fd;
}

/* Check if FILE is a regular file. If false, return 0 and
   write error message into ERROR_BUFFER. Otherwise return 1. */
int is_regular_file(struct file_struct *file, char *error_buffer)
{
  struct stat file_stat_info;
  int is_reg_file = 0;

  // retrieve file attributes using stat() and store it in struct file_stat_info
  if (stat(file->path, &file_stat_info) == -1)
  {
    copy_string("File cannot be accessed", error_buffer,
                sizeof("File cannot be accessed"));
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
   error, return -1 and write error message into ERROR_BUFFER.
   Otherwise return number of bytes read. */
int read_file(struct file_struct *file, char *error_buffer)
{
  int nbytes;

  if ((nbytes = read(file->fd, file->content, MAXFILEDATA)) == -1)
  {
    copy_string("File cannot be read", error_buffer,
                sizeof("File cannot be read"));
    return -1;
  }
  file->content[nbytes] = 0;

  return nbytes;
}

int write_from_file(struct file_struct *file, int bytes_read,
                    char *error_buffer)
{
  int write_bytes;

  if ((write_bytes = write(STDOUT_FILENO, file->content, bytes_read)) == -1)
  {
    copy_string("File content cannot be written", error_buffer,
                sizeof("File content cannot be written"));
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
