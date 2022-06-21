#!/usr/bin/env python3

import os
import subprocess

import scripts_paths

print(">> Creating the output directory…")

if not os.path.isdir(scripts_paths.output_directory_path):
    os.mkdir(scripts_paths.output_directory_path)

print(">> Running CMake…")

subprocess.check_call(["cmake", scripts_paths.root_directory_path, "-B", scripts_paths.output_directory_path, "-G", "Xcode"])
