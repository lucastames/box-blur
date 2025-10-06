#include "../include/jpeg.h"
#include <dirent.h>
#include <jpeglib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

jpeg_image_t *load_jpeg(char *path) {
  if (!path) {
    return NULL;
  }

  FILE *file = fopen(path, "rb");
  if (!file) {
    return NULL;
  }

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, true);
  jpeg_start_decompress(&cinfo);

  if (cinfo.output_components != 3) {
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return NULL;
  }

  pixel_t *pixels =
      malloc(cinfo.output_width * cinfo.output_height * sizeof(pixel_t));
  if (!pixels) {
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return NULL;
  }

  uint8_t *row_buf = malloc(cinfo.output_width * 3);
  if (!row_buf) {
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return NULL;
  }

  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, &row_buf, 1);

    for (int i = 0; i < cinfo.output_width; i++) {
      pixel_t *pixel =
          &pixels[(cinfo.output_scanline - 1) * cinfo.output_width + i];
      pixel->r = row_buf[i * 3];
      pixel->g = row_buf[i * 3 + 1];
      pixel->b = row_buf[i * 3 + 2];
    }
  }

  jpeg_image_t *image = malloc(sizeof(jpeg_image_t));
  if (!image) {
    jpeg_destroy_decompress(&cinfo);
    fclose(file);
    return NULL;
  }

  image->width = cinfo.output_width;
  image->height = cinfo.output_height;
  image->pixels = pixels;

  free(row_buf);
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(file);

  return image;
}

void blur_jpeg(jpeg_image_t *image, int radius) {
  if (!image) {
    return;
  }

  int w = image->width;
  int h = image->height;

  pixel_t *tmp = malloc(w * h * sizeof(pixel_t));
  if (!tmp) {
    return;
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int r = 0, g = 0, b = 0, count = 0;
      int x0 = x - radius < 0 ? 0 : x - radius;
      int x1 = x + radius >= w ? w - 1 : x + radius;
      for (int xx = x0; xx <= x1; xx++) {
        pixel_t *p = &image->pixels[y * w + xx];
        r += p->r;
        g += p->g;
        b += p->b;
        count++;
      }
      pixel_t *dst = &tmp[y * w + x];
      dst->r = r / count;
      dst->g = g / count;
      dst->b = b / count;
    }
  }

  pixel_t *out = malloc(w * h * sizeof(pixel_t));
  if (!out) {
    free(tmp);
    return;
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int r = 0, g = 0, b = 0, count = 0;
      int y0 = y - radius < 0 ? 0 : y - radius;
      int y1 = y + radius >= h ? h - 1 : y + radius;
      for (int yy = y0; yy <= y1; yy++) {
        pixel_t *p = &tmp[yy * w + x];
        r += p->r;
        g += p->g;
        b += p->b;
        count++;
      }
      pixel_t *dst = &out[y * w + x];
      dst->r = r / count;
      dst->g = g / count;
      dst->b = b / count;
    }
  }

  free(tmp);
  free(image->pixels);
  image->pixels = out;
}

int save_jpeg(jpeg_image_t *image, char *path) {
  FILE *file = fopen(path, "wb");
  if (!file) {
    return 1;
  }

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, file);

  cinfo.image_width = image->width;
  cinfo.image_height = image->height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, true);

  jpeg_start_compress(&cinfo, true);

  uint8_t *row_buf = malloc(image->width * 3);
  if (!row_buf) {
    jpeg_destroy_compress(&cinfo);
    fclose(file);
    return 1;
  }

  while (cinfo.next_scanline < image->height) {
    for (int i = 0; i < image->width; i++) {
      pixel_t pixel = image->pixels[cinfo.next_scanline * image->width + i];
      row_buf[i * 3] = pixel.r;
      row_buf[i * 3 + 1] = pixel.g;
      row_buf[i * 3 + 2] = pixel.b;
    }
    jpeg_write_scanlines(&cinfo, &row_buf, 1);
  }

  free(row_buf);
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  fclose(file);

  return 0;
}

void free_jpeg_image(jpeg_image_t *image) {
  free(image->pixels);
  free(image);
}
