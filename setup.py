import os
import sys
import subprocess

from distutils.command.install_data import install_data
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install_lib import install_lib
import builtins

builtins.__PYPRT_SETUP__ = True

cmake_executable = 'cmake'
cmake_build_type = 'RelWithDebInfo'


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

        cmake_install_command = [cmake_executable, '--build', self.distribution.cmake_build_dir, '--target', 'install']
        if sys.platform.startswith('win32'):
            cmake_install_command.extend(['--config', cmake_build_type])
        self.spawn(cmake_install_command)

        self.announce('Installing python modules', level=3)
        self.distribution.run_command('install_data')
        super().run()


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                'CMake must be installed to build the following extensions: ' +
                ', '.join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, extension: Extension):
        self.announce('Configuring CMake project', level=3)

        cmake_install_prefix = os.path.join(self.build_lib, 'PyPRT', 'pyprt')

        cmake_configure_command = [
            cmake_executable,
            '-DCMAKE_BUILD_TYPE={}'.format(cmake_build_type),
            '-DCMAKE_INSTALL_PREFIX={}'.format(cmake_install_prefix),
            '-DPYTHON_EXECUTABLE={}'.format(sys.executable),
            '-H{}'.format(extension.sourcedir),
            '-B{}'.format(self.build_temp)
        ]

        if sys.platform.startswith('win32'):
            cmake_configure_command.append('-Ax64')
        elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
            cmake_configure_command.append('-GNinja')

        self.spawn(cmake_configure_command)

        self.announce('Building binaries', level=3)
        cmake_build_command = [cmake_executable, '--build', self.build_temp]
        if sys.platform.startswith('win32'):
            cmake_build_command.extend(['--config', cmake_build_type])
        self.spawn(cmake_build_command)

        # hack to transport cmake build dir from here to InstallCMakeLibs
        self.distribution.cmake_build_dir = self.build_temp


setup(
    name='PyPRT',
    version='0.1.42',
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
