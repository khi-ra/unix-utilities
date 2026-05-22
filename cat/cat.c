#include <unistd.h>

int main(int argc, char **argv) {
  char buf[20];
  int n;
  while ((n = read(STDIN_FILENO, buf, 20) > 0))
    write(STDOUT_FILENO, buf, 20);
}
