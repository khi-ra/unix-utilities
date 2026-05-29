#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXINPUT 1024

ssize_t read_input(char *input_buf);

/* Read input from STDIN and store it in INPUT_BUF.
   Return the number of bytes read or -1 for error. */
ssize_t read_input(char *input_buf)
{
  ssize_t n = -1;

  // read MAXINPUT chars from stdin and store in buffer
  if ((n = read(STDIN_FILENO, input_buf, MAXINPUT)) == -1)
  {
    perror("read failed");
    return n;
  }

  // strip trailing '\n' and null terminate buffer
  input_buf[strcspn(input_buf, "\n")] = 0;
  return n;
}

int read_file(char *input_buf)
{
  /* i don't need to directly deal with opening & reading from dirs.
     only need to know if the file is a dir for error message */
  int fd;
  int absolute_path = input_buf[0] == '/';
  char file_bytes[MAXINPUT];

  if (absolute_path)
  {
    if ((fd = open(input_buf, O_RDONLY)) == -1)
    {
      perror("file opening failed");
      return fd;
    }

    // read(fd, void *buf, MAXINPUT);
  }
  else
  { // if relative path, get working dir's fd and pass into openat()
    int working_dir_fd = open("./", O_RDONLY);
    fd = openat(working_dir_fd, input_buf, O_RDONLY);
  }
  return fd;
}

int main(int argc, char **argv)
{
  // +1 for null termination character
  char input_buf[MAXINPUT + 1];
  ssize_t n;

  while ((n = read_input(input_buf)) > 0)
  {
    write(STDIN_FILENO, input_buf, n);
    write(STDIN_FILENO, "\n", 1);
  }
}
