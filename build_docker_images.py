# Copyright (c) 2012-2026 Esri R&D Center Zurich
import json
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

import docker
from docker.errors import BuildError


DOCKER_CLIENT = docker.from_env()
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
PYTHON_VERSIONS = ['3.10', '3.11', '3.12', '3.13']
KINDS = ['wheel', 'conda']
TAG_PREFIX_BASE = 'pyprt-base'
TAG_PREFIX = 'pyprt'


def main():
	build_base_images(get_os())
	build_images(get_os())


def build_base_images(os_):
	dockerfile = get_dockerfile(root_dir=ROOT_DIR, os_=os_, kind='base')
	match os_:
		case 'linux':
			tag = get_tag(prefix=TAG_PREFIX_BASE, os_=os_)
			build_image(dockerfile=dockerfile, tag=tag)
		case 'windows':
			tag = get_tag(prefix=TAG_PREFIX_BASE, os_=os_)
			build_image(dockerfile=dockerfile, tag=tag)
			for py_ver in PYTHON_VERSIONS:
				args = get_args(os_=os_, py_ver=py_ver, kind='base')
				tag = get_tag(prefix=TAG_PREFIX_BASE, os_=os_, py_ver=py_ver)
				build_image(dockerfile=dockerfile, tag=tag, args=args)


def build_images(os_):
	for kind in KINDS:
		for python_version in PYTHON_VERSIONS:
			dockerfile = get_dockerfile(root_dir=ROOT_DIR, os_=os_, kind=kind, py_ver=python_version)
			args = get_args(os_=os_, py_ver=python_version, kind=kind)
			tag = get_tag(prefix=TAG_PREFIX, os_=os_, py_ver=python_version, kind=kind)
			build_image(dockerfile=dockerfile, tag=tag, args=args)


def build_image(dockerfile, tag, args=None):
	print(f'Building image with {dockerfile} for {tag}')
	try:
		DOCKER_CLIENT.images.build(path=ROOT_DIR, dockerfile=dockerfile, tag=tag, buildargs=args, rm=True)
	except BuildError as be:
		for raw_item in be.build_log:
			log_item = json.loads(raw_item.strip('\r\n'))
			if 'stream' in log_item:
				print(log_item['stream'].strip('\n'))


def get_dockerfile(root_dir, os_, kind, py_ver=None):
	dockerfile = os.path.join(root_dir, 'envs', os_, kind, 'Dockerfile')
	if os_ == 'linux' and kind == 'wheel' and (py_ver == '3.10' or py_ver == '3.13'):
		dockerfile += '-custom-py'
	return dockerfile


def get_tag(prefix, os_, py_ver=None, kind=None):
	tag = f'{prefix}:{os_}'
	if py_ver:
		tag += f'-py{py_ver}'
	if kind:
		tag += f'-{kind}'
	return tag


def get_args(os_, py_ver, kind):
	args = dict(PY_VER=py_ver)
	if os_ == 'linux' and py_ver == '3.10':
		args['PY_VER_MICRO'] = '.19'
	elif os_ == 'linux' and py_ver == '3.13':
		args['PY_VER_MICRO'] = '.11'
	elif os_ == 'windows' and kind == 'base' and py_ver:
		args['BASE_IMAGE'] = f'python:{py_ver}-windowsservercore-1809'
	return args


def get_os():
	match sys.platform:
		case 'linux':
			return 'linux'
		case 'win32':
			return 'windows'
	raise 'unknown platform'


if __name__ == '__main__':
	main()
