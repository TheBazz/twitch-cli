#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

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
  struct passwd *pw = getpwuid(getuid());
  char *homePath = pw->pw_dir;
  char *streamerPath = "/.config/twitch-cli/streamers.txt";
  FILE *streamerFile = fopen(strcat(homePath, streamerPath), "r");
}
