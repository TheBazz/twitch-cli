#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <pwd.h>

#define MAX_STREAMERS 256
#define MAX_ANSWER 256
#define KW_LIVE "live_user"
#define KW_EXIST "rel=\"alt"

#define RESET "\033[0;0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define GREY "\033[0;37m"
#define BOLD "\033[1m"
#define UNDER "\033[4m"

static size_t WriteMemoryCallback (void *content, size_t size, size_t nmemb, void *userp);

struct MemoryStruct {
  char *memory;
  size_t size;
};

int main (int argc, char *argv[]) {
  struct passwd *pw = getpwuid(getuid());
  char *homePath = pw->pw_dir;
  char *streamerPath = "/.config/twitch-cli/streamers.txt";
  FILE *streamerFile = fopen(strcat(homePath, streamerPath), "r");
  if (streamerFile == NULL) {
    printf("Error, could not load file at ~%s.");
    exit(EXIT_FAILURE);
  }

  printf("%sYour twitch channels : %s\n", BOLD, RESET);
  int count = 0;
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
  for (int i = 0; i != count; ++i) {
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    char *url = (char *) malloc(sizeof(char) * (strlen(address) + strlen(streamers[i])));
    for (int j = 0; j != strlen(address); ++j) {
      url[j] = address[j];
    }
    for (int j = 0; j != strlen(streamers[i]); ++j) {
      url[j + strlen(address)] = streamers[i][j];
    }
    url[strlen(address) + strlen(streamers[i])] = '\0';
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
  int answer = -1;
  char str_answer[MAX_ANSWER];
  do {
    if (answer != -1 && answer <= 0 || answer > count + 1 || status[answer - 1] != 0) {
      printf("Error. ");
    }
    printf("What do you want to do (1-%s%d%s)? : ", UNDER, count + 1, RESET);
    fgets(str_answer, MAX_ANSWER, stdin);
    str_answer[strlen(str_answer) - 1] = '\0';
    if (strlen(str_answer) == 0) {
      exit(EXIT_SUCCESS);
    }
    answer = atoi(str_answer);
    if (answer == count + 1) {
      exit(EXIT_SUCCESS);
    }
  } while (answer <= 0 || answer > count + 1 || status[answer - 1] != 0);
  printf("Starting stream...");
  char *start = "mpv --force-seekable=yes --speed=1 --really-quiet \"https://www.twitch.tv/";
  char *out = (char *) malloc(sizeof(char) * (strlen(start) + strlen(streamers[answer - 1]) + 1));
  for (int i = 0; i != strlen(start); ++i) {
    out[i] = start[i];
  }
  for (int i = 0; i != strlen(streamers[answer - 1]); ++i) {
    out[i + strlen(start)] = streamers[answer - 1][i];
  }
  out[strlen(start) + strlen(streamers[answer - 1])] = '"';
  out[strlen(start) + strlen(streamers[answer - 1]) + 1] = '\0';
  system(out);
  for (int i = 0; i != count; ++i) {
    free(streamers[i]);
  }
  free(streamers);
  free(out);
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
