#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

int main(int argc, char *argv[]) {
  pid_t child = fork();
  execvp(argv[1], &argv[1]);
  return 0;
}
