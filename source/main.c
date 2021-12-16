#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <pwd.h>

#define MAX_STREAMERS 256

struct MemoryStruct {
  char *memory;
  size_t size;
};

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
  if (streamerFile == NULL) {
    printf("Error, could not load file at ~%s.");
    exit(EXIT_FAILURE);
  }

  printf("\033[1mYour twitch channels : \033[0;0m\n");
  char **streamers = (char **) malloc(sizeof(char *) * MAX_STREAMERS);
  short count = 0;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, streamerFile)) != -1) {
    streamers[count] = (char *) malloc(sizeof(char) * (read - 1));
    strncpy(streamers[count], line, read - 1);
    streamers[count][read - 1] = '\0';
    count++;
  }
  streamers = (char **) realloc(streamers, count * sizeof(char));
  fclose(streamerFile);
  CURL *curl_handle;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = (char *) malloc(1);
  chunk.size = 0;
}
