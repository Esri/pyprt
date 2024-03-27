# Copyright (c) 2012-2024 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   https://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

# a development helper script to quickly create venvs

import os
import sys
import platform
import venv
import argparse


def get_python_cmd(venv_dir):
    bin_segment = "Scripts" if platform.system() == "Windows" else "bin"
    py_cmd = os.path.join(venv_dir, bin_segment, "python")
    return py_cmd


def setup_venv_from_requirements(venv_dir):
    venv.create(venv_dir, with_pip=True)

    py_cmd = get_python_cmd(venv_dir)
    os.system(f"{py_cmd} -m pip install --upgrade pip")
    os.system(f"{py_cmd} -m pip install --upgrade wheel")

    env_os = "windows" if platform.system() == "Windows" else "linux"
    req_suffix = f"{sys.version_info[0]}.{sys.version_info[1]}"
    os.system(f"{py_cmd} -m pip install -r envs/{env_os}/wheel/requirements-py{req_suffix}.txt")


def main():
    venv_suf = f"{sys.version_info[0]}{sys.version_info[1]}"

    parser = argparse.ArgumentParser(description="PyPRT venv creation for development")
    parser.add_argument("--venv_path", help=f"venv location, defaults to .venv{venv_suf}", type=str,
                        default=f".venv{venv_suf}")
    args = parser.parse_args()

    setup_venv_from_requirements(args.venv_path)


if __name__ == "__main__":
    main()
