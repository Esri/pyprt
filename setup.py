# Copyright (c) 2012-2024 Esri R&D Center Zurich

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
import shutil
import sys
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


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
    def __init__(self, name, source_dir, debug=False):
        super().__init__(name, sources=[])
        self.source_dir = source_dir
        self.debug = debug


class CMakeBuild(build_ext):
    def build_extension(self, extension):
        if not isinstance(extension, CMakeExtension):
            super().build_extension(extension)
            return

        self.announce('Configuring CMake project', level=3)

        cmake = CMakeConfig(extension.debug)
        print(cmake)

        cmake_install_prefix = os.path.join(self.build_lib, 'pyprt', 'pyprt')

        cmake_configure_command = [
            cmake.cmake_executable,
            '-DCMAKE_BUILD_TYPE={}'.format(cmake.cmake_build_type),
            '-DCMAKE_INSTALL_PREFIX={}'.format(cmake_install_prefix),
            '-DPython_EXECUTABLE={}'.format(sys.executable),  # to control by FindPython
            '-H{}'.format(extension.source_dir),
            '-B{}'.format(self.build_temp),
            '-G{}'.format(cmake.cmake_generator),
            '-DCMAKE_MAKE_PROGRAM={}'.format(cmake.make_executable)
        ]

        if sys.platform.startswith('darwin'):
            cmake_configure_command.append('-DCMAKE_CXX_COMPILER=clang++')

        prt_dir = os.getenv('PRT_DIR', '')
        if prt_dir != '':
            cmake_configure_command.append('-Dprt_DIR:FILEPATH={}'.format(prt_dir))

        pybind11_dir = os.getenv('PYBIND11_DIR', '')
        if pybind11_dir != '':
            cmake_configure_command.append('-Dpybind11_DIR:FILEPATH={}'.format(pybind11_dir))

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
            build_dir = os.path.join(self.build_lib, package)

            # Always copy, even if source is older than destination, to ensure
            # that the right extensions for the current Python/platform are
            # used.
            shutil.copytree(src=build_dir, dst=package_dir, dirs_exist_ok=True)

            if ext._needs_stub:
                self.write_stub(package_dir, ext, compile=True)


setup(
    ext_modules=[CMakeExtension('pyprt.bin', 'src/cpp', debug=False)],
    cmdclass={'build_ext': CMakeBuild}
)
