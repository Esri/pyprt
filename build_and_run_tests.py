import os
import tempfile
import platform
import venv

env_dir = tempfile.TemporaryDirectory(prefix="pyprt-test-venv")
venv.create(env_dir.name, with_pip=True)

bin_segment = 'Scripts' if platform.system() == "Windows" else 'bin'
py_cmd = os.path.join(env_dir.name, bin_segment, 'python')

os.system("{} -m pip install --upgrade pip".format(py_cmd))
os.system("{} -m pip install --upgrade wheel".format(py_cmd))
os.system("{} -m pip install -r requirements.txt".format(py_cmd))

os.system("{} setup.py clean --all".format(py_cmd))
os.system("{} setup.py install".format(py_cmd))
os.system("{} tests/run_tests.py".format(py_cmd))
os.system("{} setup.py clean --all".format(py_cmd))

env_dir.cleanup()
