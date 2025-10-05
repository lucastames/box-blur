#include "../include/dir_util.h"
#include "../include/jpeg.h"
#include <dirent.h>
#include <linux/limits.h>
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

int blur_jpegs_from_directory(char *src_dir, char *dst_dir, int blur_radius) {
  file_list_t jpeg_file_list = list_jpeg_files(src_dir);
  if (jpeg_file_list.count == 0) {
    return 1;
  }

  if (create_dir_if_not_exists(dst_dir, 0755) != 0) {
    return 1;
  }

  for (int i = 0; i < jpeg_file_list.count; i++) {
    char dst_path[PATH_MAX];
    snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir,
             jpeg_file_list.entries[i].filename);

    process_jpeg_file(jpeg_file_list.entries[i].path, dst_path, blur_radius);
  }

  free_file_list(&jpeg_file_list);
  return 0;
}
