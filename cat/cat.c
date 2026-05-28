#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
//#include <errno.h>
#define MAXINPUT 1024
ssize_t get_input(char *input_buf);

ssize_t get_input(char *input_buf) {
  ssize_t n = -1;

  // read MAXINPUT chars from stdin and store in buffer
  if ((n = read(STDIN_FILENO, input_buf, MAXINPUT)) == -1) {
    perror("read failed");
    return n;
  }

  // removing trailing '\n' and null terminate buffer
  input_buf[n] = 0;
  input_buf[strcspn(input_buf, "\n")] = 0;
  return n;
}

int main() {
  // +1 for null termination character
  char input_buf[MAXINPUT+1];
  ssize_t n;

  while ((n = get_input(input_buf)) > 0) {
    write(STDIN_FILENO, input_buf, n);
    write(STDIN_FILENO, "\n", 1);
  }
}
