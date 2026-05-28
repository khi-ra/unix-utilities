#include <string.h>
#include <unistd.h>
#include <stdio.h>
#define MAXINPUT 1024
ssize_t read_input(char *input_buf);

ssize_t read_input(char *input_buf) {
  ssize_t n = -1;


  // read MAXINPUT chars from stdin and store in buffer
  if ((n = read(STDIN_FILENO, input_buf, MAXINPUT)) == -1) {
    perror("read failed");
    return n;
  }

  // strip trailing '\n' and null terminate buffer
  input_buf[n] = 0;
  input_buf[strcspn(input_buf, "\n")] = 0;
  return n;
}

int file_exists(char *input_buf) {
  /* takes input and checks if arg file eists */
  /* if path exists with `/` its absolute */

  /* i could just use opendir() instead of manually handling this
   * look into that and see what i need to do. i might also just
   * make my own version of 'opendir', ill read through what it does and see
   * if that seems reasonable.
   */
  int is_absolute_path = 0;
  int file_exists = 0;

  if (input_buf[0] == '/') {
    is_absolute_path = 1;
  }

  if (is_absolute_path) {

  }
}

int main(int argc, char **argv) {
  // +1 for null termination character
  char input_buf[MAXINPUT+1];
  ssize_t n;

  while ((n = read_input(input_buf)) > 0) {
    write(STDIN_FILENO, input_buf, n);
    write(STDIN_FILENO, "\n", 1);
  }
}
