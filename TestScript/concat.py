import re
import os, sys

source_dir = "output_log"
target_file = "all"

with open(target_file, "w") as write_file:
  dir = os.listdir(source_dir)
  dir.sort()
  for item in dir:
    with open(os.path.join(source_dir, item), "r") as read_file:
      write_file.write(read_file.read())


