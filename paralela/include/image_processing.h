#pragma once

#include "dir_util.h"

typedef struct BlurJPEGSTaskArg {
  int thread_id;
  int idx_inc;
  int blur_radius;
  char *dst_dir;
  file_list_t *files;
} blur_jpegs_task_arg_t;

void *blur_jpegs_task(void *arg);

int blur_jpegs_from_directory(char *src_dir, char *dst_dir, int blur_radius,
                              int num_threads);

int process_jpeg_file(char *src_path, char *dst_path, int blur_radius);
