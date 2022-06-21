#!/usr/bin/env python3

import os
import subprocess

import scripts_paths

path = os.path.join(scripts_paths.output_directory_path, "source", "Debug")

subprocess.check_call([os.path.join(path, "perlin")], cwd = path)
