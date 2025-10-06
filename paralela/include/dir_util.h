#pragma once

#include <stdbool.h>
#include <sys/types.h>

typedef struct FileEntry {
  char *path;
  char *filename;
} file_entry_t;

typedef struct FileList {
  file_entry_t *entries;
  size_t count;
} file_list_t;

bool is_directory(char *path);

int create_dir_if_not_exists(char *dir_path, mode_t mode);

void free_file_entry(file_entry_t *file_entry);

void free_file_list(file_list_t *list);

file_list_t list_jpeg_files(char *dir_path);
