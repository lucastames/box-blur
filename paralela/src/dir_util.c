#include "../include/dir_util.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

bool is_directory(char *path) {
  struct stat st;
  if (stat(path, &st) != 0) {
    return false;
  }

  return S_ISDIR(st.st_mode);
}

int create_dir_if_not_exists(char *dir_path, mode_t mode) {
  struct stat st;

  if (stat(dir_path, &st) == 0) {
    if (S_ISDIR(st.st_mode)) {
      return 0;
    }

    return 1;
  }

  if (mkdir(dir_path, mode) != 0) {
    return 1;
  }

  return 0;
}

file_list_t list_jpeg_files(char *dir_path) {
  file_list_t file_list = {.entries = NULL, .count = 0};

  DIR *dir = opendir(dir_path);
  if (!dir) {
    return file_list;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type != DT_REG) {
      continue;
    }

    char *ext = strrchr(entry->d_name, '.');
    if (!ext || (strcasecmp(ext, ".jpg") && strcasecmp(ext, ".jpeg"))) {
      continue;
    }

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

    file_list.count++;
    file_list.entries =
        realloc(file_list.entries, file_list.count * sizeof(file_entry_t));

    file_entry_t *file_entry = &file_list.entries[file_list.count - 1];
    file_entry->path = strdup(full_path);
    file_entry->filename = strdup(entry->d_name);
  }

  closedir(dir);
  return file_list;
}

void free_file_entry(file_entry_t *file_entry) {
  if (!file_entry) {
    return;
  }

  free(file_entry->filename);
  free(file_entry->path);
}

void free_file_list(file_list_t *list) {
  if (!list || !list->entries) {
    return;
  }

  for (size_t i = 0; i < list->count; i++) {
    free_file_entry(&list->entries[i]);
  }

  free(list->entries);
  list->entries = NULL;
  list->count = 0;
}
