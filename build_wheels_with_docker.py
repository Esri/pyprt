# Copyright (c) 2012-2024 Esri R&D Center Zurich
import shutil
from pathlib import Path

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

import docker
from docker.errors import ImageNotFound
from git import Repo
import os
import sys
import tempfile


ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
PYTHON_VERSIONS = ['3.10', '3.11', '3.12', '3.13']
TAG_PREFIX = 'pyprt'


def main():
	for python_version in PYTHON_VERSIONS:
		build_wheel(sys.platform, python_version)


def build_wheel(os_, py_ver):
	docker_client = docker.from_env()

	docker_image = f'pyprt:{os_}-py{py_ver}-wheel'
	try:
		docker_client.images.get(docker_image)
	except ImageNotFound as e:
		print(f'Image not found: {docker_image}, error {e}')
		return

	workdir_host = tempfile.mkdtemp(prefix=f'pyprt-py{py_ver}-')
	print(f'Working directory is {workdir_host}')
	if not copy_repo_to_workdir(os_, workdir_host):
		print(f'Failed to copy repo files to {workdir_host}')
		return

	workdir_container = '/tmp/pyprt/ws' if os_ == 'linux' else 'c:/temp/work'
	build_cmd = f'python -m build --no-isolation --wheel --outdir {workdir_container}/build'

	build_cmd_wrapper = None
	docker_env = {}
	if os_ == 'linux':
		build_cmd_wrapper = f'/bin/bash -c "{build_cmd}"'
		uid = os.getuid()
		gid = os.getgid()
		docker_env = { "DEFAULT_UID": str(uid), "DEFAULT_GID": str(gid) }
	elif os_ == 'windows':
		build_cmd_wrapper = f'cmd /c "{build_cmd}"'

	print(build_cmd_wrapper)
	try:
		docker_volumes = { workdir_host: {"bind": workdir_container, "mode": "rw"} }
		container = docker_client.containers.run(image=docker_image, command=build_cmd_wrapper, init=True, remove=True,
												 environment=docker_env, volumes=docker_volumes,
												 working_dir=workdir_container, stdout=True, stderr=True,
												 stream=True, tty=False)
		for line in container:
			print(line.decode("utf-8").strip())
		print("Container executed successfully.")
	except docker.errors.ContainerError as e:
		print(f"Container error: {e}")
	except docker.errors.ImageNotFound as e:
		print(f"Image not found: {e}")
	except docker.errors.APIError as e:
		print(f"API error: {e}")


def copy_repo_to_workdir(os_, wd):
	repo = Repo(ROOT_DIR)
	tracked_files = repo.git.ls_files().splitlines()
	for file in tracked_files:
		source = Path(ROOT_DIR, file)
		destination = Path(wd, file)
		try:
			os.makedirs(os.path.dirname(destination), exist_ok=True)
			shutil.copy2(source, destination)
		except OSError as e:
			print(f'Failed to copy {source} to {destination}')
			return False
	return True


if __name__ == '__main__':
	main()
