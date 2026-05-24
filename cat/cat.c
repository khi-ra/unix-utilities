#include <linux/limits.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
//#include<errno.h>
#define MAXINPUT 10

ssize_t get_input(char *buf);

ssize_t get_input(char* buf) {
  ssize_t n = -1;
  int max_input_exceeded = 0; // flag to set if MAXINPUT is exceeded

  if ((n = read(STDIN_FILENO, buf, MAXINPUT) == -1)) {
    perror("read failed");
    return n;
  }

  /* check if buffer has space for null termination character
   * if not, set error flag and clear characters from STDIN_FILENO
   */
  while (n > 0 && buf[n-1] != '\n' && n == MAXINPUT) {
    max_input_exceeded = 1;
    n = read(STDIN_FILENO, buf, MAXINPUT);
  }

  if (max_input_exceeded) {
    write(STDERR_FILENO, "error: input exceeded MAXINPUT\n",
      sizeof("error: input exceeded MAXINPUT\n")-1);

    return -1;
  }

  /* the issue is here somewhere. if i dont -1 it throws buffer overflow.
   * for now its throwing memory leak sanitizer issue on gdb.
   * running it normally it doesn't do anything.
   */
  buf[MAXINPUT-1] = '\0';
  return n;
}

int main() {
  char buf[MAXINPUT] = "";
  ssize_t n;

  while ((n = get_input(buf)) > 0) {
    write(STDIN_FILENO, buf, n);
  }
}
