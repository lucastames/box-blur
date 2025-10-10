#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <dir_path> <num_copies>"
  exit 1
fi

dir_path="$1"
num_copies="$2"
src_file="$dir_path/img.jpg"

if [ ! -d "$dir_path" ]; then
  echo "Error: Directory '$dir_path' does not exist."
  exit 1
fi

if [ ! -f "$src_file" ]; then
  echo "Error: File '$src_file' not found."
  exit 1
fi

if ! [[ "$num_copies" =~ ^[0-9]+$ ]] || [ "$num_copies" -lt 1 ]; then
  echo "Error: num_copies must be a positive integer."
  exit 1
fi

for ((i = 2; i <= num_copies + 1; i++)); do
  cp "$src_file" "$dir_path/img${i}.jpg"
done

echo "Successfully created $num_copies copies of img.jpg in '$dir_path'."
