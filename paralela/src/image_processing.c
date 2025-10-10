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
  blur_jpegs_task_arg_t *task = arg;

  for (int i = task->thread_id; i < task->files->count; i += task->idx_inc) {
    char dst_path[PATH_MAX];
    snprintf(dst_path, sizeof(dst_path), "%s/%s", task->dst_dir,
             task->files->entries[i].filename);

    process_jpeg_file(task->files->entries[i].path, dst_path,
                      task->blur_radius);
  }

  free(task);
  return NULL;
}

int blur_jpegs_from_directory(char *src_dir, char *dst_dir, int blur_radius,
                              int num_threads) {
  if (!src_dir || !dst_dir || blur_radius < 1 || num_threads < 1) {
    return 1;
  }

  file_list_t files = list_jpeg_files(src_dir);
  if (files.count == 0) {
    return 1;
  }

  if (create_dir_if_not_exists(dst_dir, 0755) != 0) {
    return 1;
  }

  if (num_threads > files.count) {
    num_threads = files.count;
  }

  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
  for (int i = 0; i < num_threads; i++) {
    blur_jpegs_task_arg_t *thread_arg = malloc(sizeof(blur_jpegs_task_arg_t));
    thread_arg->thread_id = i;
    thread_arg->idx_inc = num_threads;
    thread_arg->blur_radius = blur_radius;
    thread_arg->dst_dir = dst_dir;
    thread_arg->files = &files;

    pthread_create(&threads[i], NULL, blur_jpegs_task, thread_arg);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  free(threads);
  free_file_list(&files);
  return 0;
}
