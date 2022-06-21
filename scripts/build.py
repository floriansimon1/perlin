#!/usr/bin/env python3

import os
import subprocess

import scripts_paths

subprocess.check_call(["cmake", "--build", scripts_paths.output_directory_path])
