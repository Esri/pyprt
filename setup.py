import os
import sys
import subprocess

from distutils.command.install_data import install_data
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install_lib import install_lib
import builtins

builtins.__PYPRT_SETUP__ = True


class CMakeConfig:
    def __init__(self):
        self.cmake_build_type = 'RelWithDebInfo'
        self.cmake_executable = self.detect_cmake()
        self.make_executable, self.cmake_generator = self.detect_make()

    def detect_cmake(self):
        cmake_home = os.getenv('CMAKE313_HOME', '')
        cmake_candidates = [
            os.path.join(cmake_home, 'cmake'),  # 1. try env var (typically for CI)
            'cmake'                             # 2. try PATH (typically for devs)
        ]
        return self.try_alternatives('cmake', cmake_candidates)

    def detect_make(self):
        make_home = os.getenv('NINJA_HOME', '')
        cmake_generator = 'Ninja'
        make_candidates = [
            os.path.join(make_home, 'ninja'),  # 1. try env var (CI)
            'ninja',                           # 2. try PATH (devs)
            'ninja-build',                     # 3. try alternative name (e.g. used in CentOS)
            'make'                             # 4. try falling back to make if ninja is not installed
        ]
        make_executable = self.try_alternatives('ninja or make', make_candidates)
        if make_executable == 'make':
            cmake_generator = 'Unix Makefiles'
        return make_executable, cmake_generator

    def try_alternatives(self, name, candidates):
        for c in candidates:
            if self.try_run([c, '--version']):
                return c
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
    def __init__(self, name, sourcedir=''):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.join(os.path.abspath(sourcedir), 'src')


class InstallCMakeLibsData(install_data):
    def run(self):
        self.outfiles = self.distribution.data_files


class InstallCMakeLibs(install_lib):
    def run(self):
        self.announce('Installing native extension', level=3)

        cmake_install_command = [cmake.cmake_executable, '--build', self.distribution.cmake_build_dir, '--target', 'install']
        if sys.platform.startswith('win32'):
            cmake_install_command.extend(['--config', cmake.cmake_build_type])
        self.spawn(cmake_install_command)

        self.announce('Installing python modules', level=3)
        self.distribution.run_command('install_data')
        super().run()


class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, extension: Extension):
        self.announce('Configuring CMake project', level=3)

        cmake_install_prefix = os.path.join(self.build_lib, 'PyPRT', 'pyprt')

        cmake_configure_command = [
            cmake.cmake_executable,
            '-DCMAKE_BUILD_TYPE={}'.format(cmake.cmake_build_type),
            '-DCMAKE_INSTALL_PREFIX={}'.format(cmake_install_prefix),
            '-DPYTHON_EXECUTABLE={}'.format(sys.executable),
            '-H{}'.format(extension.sourcedir),
            '-B{}'.format(self.build_temp)
        ]

        if sys.platform.startswith('win32'):
            cmake_configure_command.append('-Ax64')
        elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
            cmake_configure_command.append('-GNinja')
            cmake_configure_command.append('-DCMAKE_MAKE_PROGRAM={}'.format(cmake.make_executable))

        self.spawn(cmake_configure_command)

        self.announce('Building binaries', level=3)
        cmake_build_command = [cmake.cmake_executable, '--build', self.build_temp]
        if sys.platform.startswith('win32'):
            cmake_build_command.extend(['--config', cmake.cmake_build_type])
        self.spawn(cmake_build_command)

        # hack to transport cmake build dir from here to InstallCMakeLibs
        self.distribution.cmake_build_dir = self.build_temp


cmake = CMakeConfig()
print(cmake)

setup(
    name='PyPRT',
    version='0.2.0',
    author='Camille Lechot',
    author_email='clechot@esri.com',
    description='Python bindings for the "Procedural Runtime SDK" (PRT) of "ArcGIS CityEngine" by Esri.',
    long_description='The goal of this project is to enable the execution of CityEngine rules within Python world. ',
    url='https://devtopia.esri.com/Zurich-R-D-Center/pyprt',
    platforms=['Windows', 'Linux'],
    packages=find_packages(exclude=['tests']),
    include_package_data=True,
    ext_modules=[CMakeExtension('pyprt')],
    cmdclass={
        'build_ext': CMakeBuild,
        'install_data': InstallCMakeLibsData,
        'install_lib': InstallCMakeLibs},
    zip_safe=False,
    python_requires='>=3.6'
)
