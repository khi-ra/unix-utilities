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
  char *file_content;
  char *path;
};

ssize_t read_input(char *input_buffer, struct file_struct *file);
int read_file(struct file_struct *file, char *error_buffer);
int is_regular_file(struct file_struct *file, char *error_buffer);
int open_file();
void copy_string(char *in, char *out);
int string_length(char *string);

int main(int argc, char **argv)
{
  struct file_struct file;
  // +1 for null termination character
  char input_buffer[MAXINPUT + 1];
  char *error_buffer = malloc(MAXERROR + 1);
  char file_data[MAXFILEDATA];
  int input_size;
  int file_size;

  if ((input_size = read_input(input_buffer, &file)) > 0 &&
      (file_size = read_file(&file, error_buffer)) > 0)
  {

    printf("input path: %s\n-----------\n", input_buffer);
    printf("file size: %d\n-----------\n", file_size);
    printf("file data: %s\n", file_data);
  }

  free(error_buffer);
}

/* Read input from stdin and store it in INPUT_BUFFER.
   Return the number of bytes read or -1 for error. */
ssize_t read_input(char *input_buffer, struct file_struct *file)
{
  ssize_t nbytes = -1;

  // read MAXINPUT chars from stdin and store in buffer
  if ((nbytes = read(STDIN_FILENO, input_buffer, MAXINPUT)) == -1)
  {
    perror("read failed");
    return -1;
  }

  // strip trailing '\n' and null terminate buffer
  input_buffer[strcspn(input_buffer, "\n")] = 0;

  file->path = malloc(nbytes);
  file->path = input_buffer;

  return nbytes;
}

/* Read file at FILE->path and stores content into FILE->content.
   Return number of bytes read, or -1 for error.

   Custom Errors: Writes error messages into ERROR_BUFFER passed in
   by the calling function. If read_file() returns -1, the caller shall
   check this buffer for specific information about the error.
 */
int read_file(struct file_struct *file, char *error_buffer)
{
  int dir_fd;
  int nbytes_read = -1;

  // if path is absolute, 'dir_fd' is ignored and only the path is used
  if ((dir_fd = open("./", O_RDONLY)) == -1 ||
      (file->fd = openat(dir_fd, file->path, O_RDONLY)) == -1)
  {
    perror("File cannot be opened");
  }

  if (is_regular_file(file, error_buffer))
  {
    if ((nbytes_read = read(file->fd, file->file_content, MAXFILEDATA)) == -1)
    {
      perror("File cannot be read");
    }
  }

  return nbytes_read;
}

/* Check if file at PATH is a regular file. If false, return 0 and
   write error message into ERROR_BUFFER. Otherwise return 1 */
int is_regular_file(struct file_struct *file, char *error_buffer)
{
  struct stat file_stat_info;
  int is_reg_file = 0;

  // retrieve file stat info and in struct file_stat_info
  if (stat(file->path, &file_stat_info) == -1)
  {
    copy_string("File cannot be accessed", error_buffer);
  }
  else if (S_ISDIR(file_stat_info.st_mode))
  {
    copy_string("Is a directory", error_buffer);
  }
  else if (S_ISREG(file_stat_info.st_mode))
  {
    is_reg_file = 1;
  }
  else
  {
    copy_string("Invalid file type", error_buffer);
  }
  return is_reg_file;
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
  *(out + i) = '\0';
}
