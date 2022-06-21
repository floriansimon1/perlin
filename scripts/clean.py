#!/usr/bin/env python3

import os
import distutils.dir_util

import scripts_paths

if os.path.isdir(scripts_paths.output_directory_path):
    distutils.dir_util.remove_tree(scripts_paths.output_directory_path)
