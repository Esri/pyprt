# Copyright (c) 2012-2021 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

import os
import sys
import subprocess

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.command.clean import clean
from distutils.dir_util import copy_tree, remove_tree
from distutils import log
from sphinx.setup_command import BuildDoc

try:
    from distutils.command.bdist_conda import CondaDistribution

    distclass = CondaDistribution
except:
    distclass = []

pyprt_name = 'PyPRT'
pyprt_author = 'Esri R&D Center Zurich'
pyprt_copyright = '(c) 2022, ' + pyprt_author
pyprt_version = '1.5.0rc1'  # keep consistent with __version__ in pyprt/__init__.py

record_file = os.path.join(os.path.realpath(os.curdir), pyprt_name + '.egg-info', 'record_setup_develop_files.txt')

long_description = """PyPRT provides a Python binding for PRT (Procedural RunTime) of CityEngine. This enables the 
execution of [CityEngine](https://www.esri.com/software/cityengine) CGA rules within Python. Using PyPRT, 
the generation of 3D content in Python is greatly simplified. Therefore, Python developers, data scientists, 
GIS analysts, etc. can efficiently make use of CityEngine rule packages in order to create 3D geometries stored as 
Python data structures, or to export these geometries in another format (like OBJ, Scene Layer Package, ... ). Given 
an initial geometry, on which to apply the CGA rule, the 3D generation is procedurally done in Python (Python script, 
Jupyter Notebook, ...). This allows for efficient and customizable geometry generation. For instance, when modeling 
buildings, PyPRT users can easily change the parameters of the generated buildings (like the height or the shape) by 
changing the values of the CGA rule input attributes. 

PyPRT 3D content generation is based on CGA rule packages (RPK), which are authored in CityEngine. RPKs contain the 
CGA rule files that define the shape transformations, as well as supplementary assets. RPK examples can be found 
below and directly used in PyPRT. 

PyPRT allows generating 3D models on multiple initial geometries. Different input attributes can be applied on each 
of these initial shapes. Moreover, the outputted 3D geometries can either be used inside Python or exported to 
another format by using one of PRT encoders."""


class CMakeConfig:
    def __init__(self, is_debug):
        self.cmake_build_type = 'RelWithDebInfo' if is_debug else 'Release'
        self.cmake_executable = self.detect_cmake()
        self.make_executable, self.cmake_generator = self.detect_make()

    def detect_cmake(self):
        cmake_home = os.getenv('CMAKE313_HOME', '')
        cmake_candidates = [
            # 1. try env var (typically for CI)
            [os.path.join(cmake_home, 'cmake'), '--version'],
            # 2. try PATH (typically for devs)
            ['cmake', '--version']
        ]
        return self.try_alternatives('cmake', cmake_candidates)

    def detect_make(self):
        make_home = os.getenv('NINJA_HOME', '')
        make_candidates = [
            # 1. try env var
            [os.path.join(make_home, 'ninja'), '--version'],
            # 2. try PATH with ninja
            ['ninja', '--version'],
            # 3. try PATH with alternative name (e.g. used in CentOS)
            ['ninja-build', '--version'],
            # 4. try PATH with make (macos, linux)
            ['make', '--version'],
            # 5. try PATH with nmake (windows)
            ['nmake', '/?']
        ]
        make_executable = self.try_alternatives(
            'ninja or (n)make', make_candidates)

        # derive cmake generator from detected make tool
        cmake_generator = None
        if 'ninja' in make_executable:
            cmake_generator = 'Ninja'
        elif make_executable == 'make':
            cmake_generator = 'Unix Makefiles'
        elif make_executable == 'nmake':
            cmake_generator = 'NMake Makefiles'

        return make_executable, cmake_generator

    def try_alternatives(self, name, candidates):
        for c in candidates:
            if self.try_run(c):
                return c[0]
        raise RuntimeError('Cannot find executable for {}!'.format(name))

    @staticmethod
    def try_run(cmd):
        try:
            subprocess.check_output(cmd)
        except OSError:
            return False
        return True

    def __str__(self):
        return """CMake Configuration:
    cmake           : {}
    cmake generator : {}
    make            : {}""".format(self.cmake_executable, self.cmake_generator, self.make_executable)


class CMakeExtension(Extension):
    def __init__(self, name, source_dir):
        super().__init__(name, sources=[])
        self.source_dir = source_dir


class CMakeBuild(build_ext):
    def build_extension(self, extension):
        if not isinstance(extension, CMakeExtension):
            super().build_extension(extension)
            return

        self.announce('Configuring CMake project', level=3)

        cmake = CMakeConfig(self.debug)
        print(cmake)

        cmake_install_prefix = os.path.join(self.build_lib, 'pyprt', 'pyprt')

        cmake_configure_command = [
            cmake.cmake_executable,
            '-DCMAKE_BUILD_TYPE={}'.format(cmake.cmake_build_type),
            '-DCMAKE_INSTALL_PREFIX={}'.format(cmake_install_prefix),
            '-DPYTHON_EXECUTABLE={}'.format(sys.executable),
            '-H{}'.format(extension.source_dir),
            '-B{}'.format(self.build_temp),
            '-G{}'.format(cmake.cmake_generator),
            '-DCMAKE_MAKE_PROGRAM={}'.format(cmake.make_executable)
        ]

        if sys.platform.startswith('darwin'):
            cmake_configure_command.append('-DCMAKE_CXX_COMPILER=clang++')

        prt_dir = os.getenv('PRT_DIR', '')
        if prt_dir != '':
            cmake_configure_command.append('-Dprt_DIR={}'.format(prt_dir))

        self.spawn(cmake_configure_command)

        self.announce('Building binaries', level=3)
        cmake_build_command = [
            cmake.cmake_executable, '--build', self.build_temp]
        if sys.platform.startswith('win32'):
            cmake_build_command.extend(['--config', cmake.cmake_build_type])
        self.spawn(cmake_build_command)

        # now let's do our cmake thing
        self.announce('Installing native extension', level=3)
        cmake_install_command = [
            cmake.cmake_executable,
            '--build', self.build_temp,
            '--target', 'install'
        ]
        if sys.platform.startswith('win32'):
            cmake_install_command.extend(['--config', cmake.cmake_build_type])
        self.spawn(cmake_install_command)

    def copy_extensions_to_source(self):
        build_py = self.get_finalized_command('build_py')
        for ext in self.extensions:
            fullname = self.get_ext_fullname(ext.name)
            modpath = fullname.split('.')
            package = '.'.join(modpath[:-1])
            package_dir = build_py.get_package_dir(package)

            # Always copy, even if source is older than destination, to ensure
            # that the right extensions for the current Python/platform are
            # used.
            files_record = copy_tree(self.build_lib, os.curdir,
                                     verbose=self.verbose, dry_run=self.dry_run)
            with open(record_file, 'w+') as f:
                for each_file in files_record:
                    f.write('%s\n' % each_file)
            f.close()

            if ext._needs_stub:
                self.write_stub(package_dir or os.curdir, ext, True)


class CleanCommand(clean):
    def run(self):
        clean.run(self)
        if os.path.isfile(record_file):
            with open(record_file, 'r') as f:
                for each_file in f:
                    fname = os.path.join(each_file.rstrip())
                    dirname = os.path.dirname(fname)
                    basename = os.path.basename(dirname)
                    if os.path.exists(dirname) and (basename == 'bin' or basename == 'lib'):
                        remove_tree(dirname, dry_run=self.dry_run)
                    else:
                        if os.path.isfile(fname):
                            os.remove(fname)
                            log.warn("removing '%s'", fname)
            os.remove(os.path.join(os.curdir, record_file))
            log.warn("removing '%s'", record_file)


setup(
    name=pyprt_name,
    version=pyprt_version,
    author=pyprt_author,
    description='Python bindings for the "Procedural Runtime" (PRT) of CityEngine by Esri.',
    long_description_content_type="text/markdown",
    long_description=long_description,
    url='https://esri.github.io/cityengine/pyprt',
    download_url='https://github.com/Esri/pyprt/releases',
    project_urls={"Documentation": "https://github.com/Esri/pyprt/blob/master/README.md#documentation",
                  "Examples": "https://github.com/Esri/pyprt-examples",
                  "Source Code": "https://github.com/Esri/pyprt"},
    platforms=['Windows', 'Linux'],
    packages=find_packages(exclude=['tests']),
    include_package_data=True,
    ext_modules=[CMakeExtension('pyprt.pyprt', 'src')],
    cmdclass={'build_ext': CMakeBuild, 'clean': CleanCommand,
              'build_doc': BuildDoc},
    distclass=distclass,
    conda_import_tests=False,
    license="PyPRT is free for personal, educational, and non-commercial use. Commercial use requires at least one "
            "commercial license of the latest CityEngine version installed in the organization. Redistribution or web "
            "service offerings are not allowed unless expressly permitted. PyPRT is under the same license as the "
            "included [CityEngine SDK](https://github.com/Esri/esri-cityengine-sdk#licensing). An exception is the "
            "PyPRT source code (without CityEngine SDK, binaries, or object code), which is licensed under the Apache "
            "License, Version 2.0 (the “License”); you may not use this work except in compliance with the License. "
            "You may obtain a copy of the License at https://www.apache.org/licenses/LICENSE-2.0",
    classifiers=['Development Status :: 5 - Production/Stable',
                 'License :: Free for non-commercial use',
                 'License :: OSI Approved :: Apache Software License',
                 'Operating System :: Microsoft :: Windows',
                 'Operating System :: Unix',
                 'Programming Language :: C++',
                 'Programming Language :: Python',
                 'Programming Language :: Python :: 3 :: Only',
                 'Topic :: Multimedia :: Graphics :: 3D Modeling',
                 'Topic :: Scientific/Engineering',
                 'Topic :: Software Development :: Libraries :: Python Modules'],
    zip_safe=False,
    python_requires='>=3.6',
    command_options={
        'build_doc': {
            'project': ('setup.py', pyprt_name),
            'copyright': ('setup.py', pyprt_copyright),
            'version': ('setup.py', pyprt_version),
            'release': ('setup.py', pyprt_version),
            'source_dir': ('setup.py', 'docs')}},
)
