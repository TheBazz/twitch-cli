#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[]) {
  size_t pathEnd;
  for (size_t i = strlen(argv[0]); i != 0; --i) {
    if (argv[0][i] == '/') {
      pathEnd = i;
      break;
    }
  }
  char *path = (char *) malloc(sizeof(char) * pathEnd);
  strncpy(path, argv[0], pathEnd);
}
