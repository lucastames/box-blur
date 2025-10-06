#include "../include/dir_util.h"
#include "../include/image_processing.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <SRC_DIR> <DST_DIR> <BLUR_RADIUS>\n", argv[0]);
    return 1;
  }

  char *src_dir = argv[1];
  char *dst_dir = argv[2];
  int radius = atoi(argv[3]);

  if (radius < 1) {
    fprintf(stderr, "Invalid BLUR_RADIUS: %d \n", radius);
    return 1;
  }

  if (!is_directory(src_dir)) {
    fprintf(stderr, "Invalid SRC_DIR: %s isn't a directory\n", src_dir);
    return 1;
  }

  return blur_jpegs_from_directory(src_dir, dst_dir, radius);
}
