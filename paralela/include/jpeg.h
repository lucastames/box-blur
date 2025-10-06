#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct Pixel {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel_t;

typedef struct JPEGImage {
  int width;
  int height;
  pixel_t *pixels;
} jpeg_image_t;

jpeg_image_t *load_jpeg(char *path);

void blur_jpeg(jpeg_image_t *image, int radius);

int save_jpeg(jpeg_image_t *image, char *path);

void free_jpeg_image(jpeg_image_t *image);
