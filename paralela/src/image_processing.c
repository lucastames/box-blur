#include "../include/image_processing.h"
#include "../include/dir_util.h"
#include "../include/jpeg.h"
#include <bits/pthreadtypes.h>
#include <dirent.h>
#include <linux/limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int process_jpeg_file(char *src_path, char *dst_path, int blur_radius) {
  jpeg_image_t *image = load_jpeg(src_path);
  if (!image) {
    return 1;
  }

  blur_jpeg(image, blur_radius);
  save_jpeg(image, dst_path);
  free_jpeg_image(image);
  return 0;
}

void *blur_jpegs_task(void *arg) {
  blur_jpegs_task_arg_t *task_arg = arg;
  for (int i = task_arg->start_idx; i < task_arg->end_idx; i++) {
    char dst_path[PATH_MAX];
    snprintf(dst_path, sizeof(dst_path), "%s/%s", task_arg->dst_dir,
             task_arg->entries[i].filename);

    process_jpeg_file(task_arg->entries[i].path, dst_path,
                      task_arg->blur_radius);
  }
  free(task_arg);
  return NULL;
}

int blur_jpegs_from_directory(char *src_dir, char *dst_dir, int blur_radius) {
  const int MAX_THREADS = 10;

  file_list_t jpeg_file_list = list_jpeg_files(src_dir);
  if (jpeg_file_list.count == 0) {
    return 1;
  }

  if (create_dir_if_not_exists(dst_dir, 0755) != 0) {
    return 1;
  }

  int num_threads =
      jpeg_file_list.count < MAX_THREADS ? jpeg_file_list.count : MAX_THREADS;
  int files_per_thread = jpeg_file_list.count / num_threads;
  int remainder_files = jpeg_file_list.count % num_threads;
  pthread_t threads[num_threads];

  int start = 0;
  for (int i = 0; i < num_threads; i++) {
    int end = start + files_per_thread + (i < remainder_files ? 1 : 0);

    blur_jpegs_task_arg_t *thread_arg = malloc(sizeof(blur_jpegs_task_arg_t));
    thread_arg->start_idx = start;
    thread_arg->end_idx = end;
    thread_arg->blur_radius = blur_radius;
    thread_arg->dst_dir = dst_dir;
    thread_arg->entries = jpeg_file_list.entries;

    pthread_create(&threads[i], NULL, blur_jpegs_task, thread_arg);
    start = end;
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  free_file_list(&jpeg_file_list);
  return 0;
}
