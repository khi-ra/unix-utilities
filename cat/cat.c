#include <linux/limits.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
//#include <errno.h>
#define MAXINPUT 11

ssize_t get_input(char *buf);

ssize_t get_input(char* buf) {
  ssize_t n = -1;
  int max_input_exceeded = 0; // flag to set if MAXINPUT is exceeded

  /* !issue1:
   * read() returns 0 (EOF) instead # bytes read,
   * can't null-terminate buf,
   * calling function can't use 'n' to read bytes,
   * gdb shows read bytes stored in buf[] and n = 0
   */
  // attempt to read MAXINPUT chars at from standard input
  if ((n = read(STDIN_FILENO, buf, 1) == -1)) {
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

  buf[2] = '\0';
  return n;
}

int main() {
  char buf[MAXINPUT] = "";
  ssize_t n;

  /* !issue1 */
  while ((n = get_input(buf)) > 0) {
    write(STDIN_FILENO, buf, 1);
  }
}
