#pragma once

int blur_jpegs_from_directory(char *src_dir, char *dst_dir, int blur_radius);

int process_jpeg_file(char *src_path, char *dst_path, int blur_radius);
