import os

script_directory_path = os.path.dirname(os.path.realpath(__file__))

root_directory_path   = os.path.realpath(os.path.join(script_directory_path, ".."))

output_directory_path = os.path.join(root_directory_path, "build")
