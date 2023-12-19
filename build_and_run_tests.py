# Copyright (c) 2012-2023 Esri R&D Center Zurich

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

import os
import sys
import tempfile
import platform
import venv

env_os = "windows" if platform.system() == "Windows" else "linux"
env_py = f"py{sys.version_info[0]}.{sys.version_info[1]}"

env_dir = tempfile.TemporaryDirectory(prefix="pyprt-test-venv")
venv.create(env_dir.name, with_pip=True)

bin_segment = "Scripts" if platform.system() == "Windows" else "bin"
py_cmd = os.path.join(env_dir.name, bin_segment, 'python')

os.system(f"{py_cmd} -m pip install --upgrade pip")
os.system(f"{py_cmd} -m pip install --upgrade wheel")
os.system(f"{py_cmd} -m pip install -r envs/{env_os}/wheel/requirements-{env_py}.txt")

os.system(f"{py_cmd} -m pip install .")
os.system(f"{py_cmd} tests/run_tests.py")
os.system(f"{py_cmd} -m pip uninstall -y pyprt")

env_dir.cleanup()
