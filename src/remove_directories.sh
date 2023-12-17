#!/bin/bash

# Define the parent directories
parent_directories=(
  "../ASDIR/USERS"
  "../ASDIR/AUCTIONS"
)

# Loop through each parent directory and remove its subdirectories
for parent_dir in "${parent_directories[@]}"; do
  if [ -d "$parent_dir" ]; then
    echo "Removing subdirectories in $parent_dir"
    find "$parent_dir" -mindepth 1 -maxdepth 1 -type d -exec rm -r {} \;
  else
    echo "Parent directory does not exist: $parent_dir"
  fi
done
