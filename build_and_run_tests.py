import os
import sys
import tempfile
import platform
import venv

env_os = "windows" if platform.system() == "Windows" else "centos7"
env_py = f"py{sys.version_info[0]}{sys.version_info[1]}"

env_dir = tempfile.TemporaryDirectory(prefix="pyprt-test-venv")
venv.create(env_dir.name, with_pip=True)

bin_segment = "Scripts" if platform.system() == "Windows" else "bin"
py_cmd = os.path.join(env_dir.name, bin_segment, 'python')

os.system(f"{py_cmd} -m pip install --upgrade pip")
os.system(f"{py_cmd} -m pip install --upgrade wheel")
os.system(f"{py_cmd} -m pip install -r envs/{env_os}/{env_py}/requirements.txt")

os.system(f"{py_cmd} setup.py clean --all")
os.system(f"{py_cmd} setup.py install")
os.system(f"{py_cmd} tests/run_tests.py")
os.system(f"{py_cmd} setup.py clean --all")

env_dir.cleanup()
