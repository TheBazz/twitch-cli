#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <pwd.h>

#define MAX_STREAMERS 256
#define KW_LIVE "live_user"
#define KW_EXIST "rel=\"alt"

#define RESET "\033[0;0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BOLD "\033[1m"
#define GREY "\033[0;37m"

static size_t WriteMemoryCallback (void *content, size_t size, size_t nmemb, void *userp);

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

  printf("%sYour twitch channels : %s\n", BOLD, RESET);
  short count = 0;
  char **streamers = (char **) malloc(sizeof(char *) * MAX_STREAMERS);
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&line, &len, streamerFile)) != -1) {
    streamers[count] = (char *) malloc(sizeof(char) * (read - 1));
    strncpy(streamers[count], line, read - 1);
    streamers[count][read - 1] = '\0';
    count++;
  }
  streamers = (char **) realloc(streamers, sizeof(char *) * count);
  char *status = (char *) malloc(sizeof(char) * count);
  fclose(streamerFile);
  CURL *curl_handle;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = (char *) malloc(1);
  chunk.size = 0;
  char *address = "https://www.twitch.tv/";
  for (short i = 0; i != count; ++i) {
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    char *url = (char *) malloc(sizeof(char) * (strlen(address) + strlen(streamers[i])));
    for (int j = 0; j != strlen(address); ++j) {
      *(url + j) = *(address + j);
    }
    for (int j = 0; j != strlen(streamers[i]); ++j) {
      *(url + j + strlen(address)) = *(streamers[i] + j);
    }
    *(url + strlen(address) + strlen(streamers[i])) = '\0';
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    free(url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
      printf("Error, curl failed.");
      free(chunk.memory);
      exit(EXIT_FAILURE);
    }
    char *output = chunk.memory;
    if (strstr(output, KW_LIVE)) {
      status[i] = 0;
    } else if (strstr(output, KW_EXIST)) {
      status[i] = 1;
    } else {
      status[i] = 2;
    }
    free(chunk.memory);
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;
  }
  free(chunk.memory);
  for (int i = 0; i != count; i++) {
    if (status[i] == 0) {
      printf(" %d - %s : %slive%s\n", i + 1, streamers[i], GREEN, RESET);
    } else if (status[i] == 1) {
      printf(" %d - %s : %soffline%s\n", i + 1, streamers[i], GREY, RESET);
    } else {
      printf(" %d - %s : %serror%s\n", i + 1, streamers[i], RED, RESET);
    }
  }
  printf(" %d - Exit\n", count + 1);
}

static size_t WriteMemoryCallback (void *content, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *) userp;
  char *ptr = (char *) realloc(mem->memory, mem->size + realsize + 1);
  if (!ptr) {
    printf("Error, no memory available!");
    exit(EXIT_FAILURE);
  }
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), content, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}
