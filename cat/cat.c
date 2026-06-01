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
  int size;
};

ssize_t read_input(struct file_struct *file);
int read_file(struct file_struct *file, char *error_buffer);
int is_regular_file(struct file_struct *file, char *error_buffer);
int open_file(struct file_struct *file);
void copy_string(char *in, char *out); /* look into strcpy */

/* Custom Error messages: Any function that takes ERROR_BUFFER as an arg writes
   an error message into it. If said function then returns -1, the caller should
   check this buffer for specific information about the error. */

/* Accept files as command-line input and display their content out to standard
   output. */
int main(int argc, char **argv)
{
  struct file_struct file;
  int input_size;
  int file_size;
  // +1 for null termination
  char *error_buffer = malloc(MAXERROR + 1);
  file.path = malloc(MAXINPUT + 1);
  file.content = malloc(MAXFILEDATA + 1);
  file.size = 0;

  if ((input_size = read_input(&file)) > 0 &&
      (file_size = read_file(&file, error_buffer)) > 0)
  {
    printf("input path: %s\n-----------\n", file.path);
    printf("file size: %d\n-----------\n", file_size);
    printf("file content: %s\n", file.content);
  }
  else
  {
    printf("Error: %s\n", error_buffer);
  }

  free(error_buffer);
  free(file.path);
  free(file.content);
}

/* Read input from stdin and store it in FILE->path.
   Return the number of bytes read or -1 for error. */
ssize_t read_input(struct file_struct *file)
{
  ssize_t nbytes = -1;

  // read MAXINPUT chars from stdin and store in buffer
  if ((nbytes = read(STDIN_FILENO, file->path, MAXINPUT)) == -1)
  {
    perror("read failed");
    return -1;
  }

  // strip trailing '\n' and null terminate buffer
  int path_length = strcspn(file->path, "\n");
  *(file->path + path_length) = 0;
  file->path = realloc(file->path, nbytes);

  return nbytes;
}

/* Read file at FILE->path and store it's content into FILE->content. Upon
   error, return -1 and write error message into ERROR_BUFFER.
   Otherwise return number of bytes read. */
int read_file(struct file_struct *file, char *error_buffer)
{
  char file_content_buffer[MAXFILEDATA + 1];
  int nbytes_read;

  if ((file->fd = open_file(file)) == -1)
  {
    copy_string("File cannot be opened", error_buffer);
    return -1;
  }

  if (!is_regular_file(file, error_buffer))
  {
    return -1;
  }

  if ((nbytes_read = read(file->fd, file_content_buffer, MAXFILEDATA)) == -1)
  {
    copy_string("File cannot be read", error_buffer);
    return -1;
  }
  file->size += nbytes_read;

  /* For copying large blocks of data use memcpy()/strcpy() instead of
     copy_string(), it's much more efficient: memcpy(*to, *from, sizeof(*from))

     However these functions are not safe, need to ensure certain criteria are
     met before using them. */
  if (file->size < MAXFILEDATA)
  {
    // copy buffer content to file and free empty space
    copy_string(file_content_buffer, file->content);
    file->content = realloc(file->content, file->size);
  }
  else if (nbytes_read == MAXFILEDATA &&
           ((int)file_content_buffer[nbytes_read]) != EOF)
  {
    int temp = MAXFILEDATA;
    copy_string(file_content_buffer, file->content);
    // allocate an additional MAXFILEDATA bytes
    file->content = realloc(file->content, file->size + MAXFILEDATA);
    nbytes_read = read(file->fd, file_content_buffer, MAXFILEDATA);
    file->size += nbytes_read;
  }
  *(file->content + file->size) = 0;

  return nbytes_read;
}

/* Check if file at PATH is a regular file. If false, return 0 and
   write error message into ERROR_BUFFER. Otherwise return 1. */
int is_regular_file(struct file_struct *file, char *error_buffer)
{
  struct stat file_stat_info;
  int is_reg_file = 0;

  // retrieve file attributes using stat() and store it in struct file_stat_info
  if (stat(file->path, &file_stat_info) == -1)
  {
    copy_string("File cannot be accessed", error_buffer);
    return is_reg_file;
  }

  if (S_ISREG(file_stat_info.st_mode))
  {
    is_reg_file = 1;
  }
  else if (S_ISDIR(file_stat_info.st_mode))
  {
    copy_string("Is a directory", error_buffer);
  }
  else
  {
    copy_string("Invalid file type", error_buffer);
  }
  return is_reg_file;
}

/* Open file in read-only mode and return it's file descriptor, or -1 for
   error. */
int open_file(struct file_struct *file)
{
  int dir_fd = open("./", O_RDONLY);

  // if path is absolute, 'dir_fd' is ignored and only the path is used
  if (dir_fd == -1 || (file->fd = openat(dir_fd, file->path, O_RDONLY)) == -1)
  {
    return -1;
  }

  close(dir_fd);
  return file->fd;
}

/* Copy contents of IN into OUT. */
void copy_string(char *in, char *out)
{
  int i = 0;
  while (*(in + i) != '\0')
  {
    *(out + i) = *(in + i);
    i++;
  }
  // null terminate 'out'
  *(out + i) = 0;
}
