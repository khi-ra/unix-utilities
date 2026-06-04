#include <endian.h>
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
  char *content;
  char *path;
  size_t size;
  off_t offset;
};

ssize_t read_input(struct file_struct *file);
int open_file(struct file_struct *file, char *error_buffer);
int is_regular_file(struct file_struct *file, char *error_buffer);
int read_file(struct file_struct *file, char *error_buffer);
void copy_string(char *out, char *in, size_t in_size);

/* Custom error messages: Any function that takes ERROR_BUFFER as an arg writes
   an error message into it. If said function returns -1, the caller should
   check this buffer for specific information about the error. */

/* Accept files as command-line input and display their content out to standard
   output. */
int main(int argc, char **argv)
{
  struct file_struct file;
  int input;
  int read_bytes;

  // +1 for null termination
  char error_buffer[MAXERROR + 1];
  file.content = malloc(MAXFILEDATA + 1);
  file.path = malloc(MAXINPUT + 1);
  file.size = 0;

  if ((input = read_input(&file)) == -1)
  {
    printf("Error: %s\n", error_buffer);
    return -1;
  }
  else if ((file.fd = open_file(&file, error_buffer)) == -1)
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
    write(STDIN_FILENO, file.content, read_bytes);
  }
  if (read_bytes == -1)
  {
    printf("Error: %s\n", error_buffer);
    return -1;
  }

  free(file.path);
  free(file.content);
}

/* Read input from stdin and store it in FILE->path.
   Return the number of bytes read or -1 for error. */
ssize_t read_input(struct file_struct *file)
{
  ssize_t nbytes = -1;

  if ((nbytes = read(STDIN_FILENO, file->path, MAXINPUT)) == -1)
  {
    perror("read failed");
    return nbytes;
  }
  file->path = realloc(file->path, nbytes);

  *(file->path + (nbytes - 1)) = 0;
  return nbytes;
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
  *(file->content + nbytes) = 0;

  return nbytes;
}

/* Copy contents of IN to OUT */
void copy_string(char *in, char *out, size_t in_size)
{
  memmove(out, in, in_size);
  *(out + in_size) = 0;
}
