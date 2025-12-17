# PyPRT - Python Bindings for the Procedural Runtime (PRT) of ArcGIS CityEngine
# Copyright (c) 2012-2025 Esri R&D Center Zurich
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# A copy of the license is available in the repository's LICENSE file.

# helper script to fetch built packages from CI and publish them on PyPI and/or Anaconda
# (this script is geared towards Esri-internal CI infrastructure)

# typical usage:
# $ .venv/bin/python publish_packages.py --nexus_service https://<host>/nexus --nexus_group pyprt/release-X
#     --nexus_version 1.11.0.4.dev0 --publish_kind both

import argparse
import os
import ssl
import tempfile
from pathlib import Path
from urllib.request import urlopen
import subprocess
import keyring
import sys

PKG_PYTHON_VERSIONS = ['310', '311', '312', '313']

PKG_KIND_WHEEL = 'wheel'
PKG_KIND_CONDA = 'conda'
PKG_KINDS = [PKG_KIND_WHEEL, PKG_KIND_CONDA]

PKG_OS_LINUX = 'linux'
PKG_OS_WINDOWS = 'windows'
PKG_OSES = [PKG_OS_LINUX, PKG_OS_WINDOWS]

PKG_WHEEL_CLS_WINDOWS = 'win_amd64'
PKG_WHEEL_CLS_LINUX = 'manylinux_2_28_x86_64'
PKG_CONDA_CLS_WINDOWS = 'win10-x86_64'
PKG_CONDA_CLS_LINUX = 'rhel8-x86_64'
PKG_WHEEL_EXT = '.whl'
PKG_CONDA_EXT = '.tar.bz2'

PUBLISH_KIND_NONE = 'none'
PUBLISH_KIND_BOTH = 'both'
PUBLISH_KINDS = PKG_KINDS + [PUBLISH_KIND_NONE, PUBLISH_KIND_BOTH]

PUBLISH_MODE_TEST = 'test'
PUBLISH_MODE_PROD = 'prod'
PUBLISH_MODES = [PUBLISH_MODE_TEST, PUBLISH_MODE_PROD]

KEYRING_CRED_NAME_PYPI = 'pypi.org'
KEYRING_CRED_NAME_TESTPYPI = 'test.pypi.org'
KEYRING_CRED_NAME_ANACONDA = 'anaconda.org'


def main():
    args = get_args()
    if os.path.exists(args.workdir):
        raise Exception(f'Work directory {args.workdir} already exists')

    fetch_packages(args)
    print(f'Packages have been fetched to {args.workdir}')

    if args.publish_kind in [PKG_KINDS, PUBLISH_KIND_BOTH]:
        publish(args)
        print(f'Packages have been published to {args.publish_kind} with mode {args.publish_mode}')


def publish(args):
    if args.publish_kind in [PKG_KIND_WHEEL, PUBLISH_KIND_BOTH]:
        service_name = KEYRING_CRED_NAME_PYPI if args.publish_mode == PUBLISH_MODE_PROD else KEYRING_CRED_NAME_TESTPYPI
        pypi_api_token = keyring.get_credential(service_name=service_name, username=None)
        if pypi_api_token:
            pypi_target = 'pypi' if args.publish_mode == PUBLISH_MODE_PROD else 'testpypi'
            bin_path = Path(sys.executable).parent
            twine_path = bin_path / 'twine'
            subprocess.run([twine_path, 'upload', '-u', '__token__', '-p', pypi_api_token.password, '--non-interactive',
                            '--disable-progress-bar', '--repository', pypi_target, f'{args.workdir}/wheel/*.whl'])
        else:
            raise Exception(f'Could not get keyring credentials for {service_name}')

    if args.publish_kind in [PKG_KIND_CONDA, PUBLISH_KIND_BOTH]:
        # note: ignoring args.publish_mode (there is no anaconda test repo)
        pkgs = list((Path(args.workdir) / 'conda').glob('**/*.tar.bz2'))
        anaconda_creds = keyring.get_credential(service_name=KEYRING_CRED_NAME_ANACONDA, username=None)
        if anaconda_creds:
            anaconda_cmd_base = ['conda', 'run', 'anaconda']
            subprocess.run(anaconda_cmd_base + ['logout'])
            subprocess.run(anaconda_cmd_base + ['login', '--username', anaconda_creds.username, '--password',
                                                anaconda_creds.password])
            subprocess.run(anaconda_cmd_base + ['upload', '--user', 'Esri'] + pkgs)
        else:
            raise Exception('Could not get keyring credentials for "anaconda"')


def fetch_packages(args):
    for kind in PKG_KINDS:
        for py_ver in PKG_PYTHON_VERSIONS:
            for os_ in PKG_OSES:
                fetch(args, kind, py_ver, os_)


def fetch(args, kind, py_ver, os_):
    url = get_url(args, kind, py_ver, os_)
    local_path = get_local_path(args, kind, py_ver, os_)
    print(f'Fetching {url} to {local_path}...')

    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE

    with urlopen(url, context=ctx) as connection:
        data = connection.read()
    with open(local_path, 'wb') as file_object:
        file_object.write(data)

    print('... done.')


def get_local_path(args, kind, py_ver, os_):
    local_path = Path(args.workdir) / kind
    if kind == PKG_KIND_CONDA:
        local_path /= os_
    local_path.mkdir(parents=True, exist_ok=True)

    if kind == PKG_KIND_WHEEL:  # pyprt-1.11.0.150.dev0-cp39-cp39-win_amd64.whl
        local_path /= f'pyprt-{args.nexus_version}-{get_classifier(kind, py_ver, os_)}{PKG_WHEEL_EXT}'
    elif kind == PKG_KIND_CONDA:  # pyprt-1.1.0-py36_34.tar.bz2, with "34" being the build number
        tok_ver = args.nexus_version.split('.')
        ver_bld = tok_ver[3]  # conda package names want build number again at the end
        local_path /= f'pyprt-{args.nexus_version}-py{py_ver}_{ver_bld}{PKG_CONDA_EXT}'

    return local_path


def get_url(args, kind, py_ver, os_):
    ext = PKG_WHEEL_EXT if kind == PKG_KIND_WHEEL else PKG_CONDA_EXT
    url = f'{args.nexus_service}/repository/{args.nexus_repo}/{args.nexus_group}/pyprt/{args.nexus_version}/'
    url += f'pyprt-{args.nexus_version}-{get_classifier(kind, py_ver, os_)}{ext}'
    return url


def get_classifier(kind, py_ver, os_):
    if kind == PKG_KIND_WHEEL:
        return f'cp{py_ver}-cp{py_ver}-' + (PKG_WHEEL_CLS_LINUX if os_ == PKG_OS_LINUX else PKG_WHEEL_CLS_WINDOWS)
    elif kind == PKG_KIND_CONDA:
        return f'py{py_ver}-' + (PKG_CONDA_CLS_LINUX if os_ == PKG_OS_LINUX else PKG_CONDA_CLS_WINDOWS)


def get_args():
    parser = argparse.ArgumentParser(description='Helper script to publish PyPRT packages to PyPI and/or Anaconda')
    parser.add_argument('--nexus_service', type=str, required=True,
                        help="The Nexus service URL, for example https://<hostname>/nexus")
    parser.add_argument('--nexus_repo', type=str, default='CityEngine')
    parser.add_argument('--nexus_group', type=str, default='prt-apps/pyprt/main')
    parser.add_argument('--nexus_version', type=str, required=True)
    parser.add_argument('--workdir', type=str, required=False)
    parser.add_argument('--publish_kind', type=str, default=PUBLISH_KIND_NONE, choices=PUBLISH_KINDS)
    parser.add_argument('--publish_mode', type=str, default=PUBLISH_MODE_TEST, choices=PUBLISH_MODES)

    args = parser.parse_args()
    if not args.workdir:
        args.workdir = os.path.join(tempfile.gettempdir(), f'pyprt_pkg_{args.nexus_version}')

    return args


if __name__ == '__main__':
    main()
