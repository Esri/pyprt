import os
import re
import sys
import sysconfig
import platform
import subprocess

from distutils.command.install_data import install_data
from setuptools import setup, Extension, find_packages, Distribution
from setuptools.command.build_ext import build_ext
from setuptools.command.install_lib import install_lib
from setuptools.command.install_scripts import install_scripts
from setuptools.command.test import test
import pathlib
import shutil
import unittest

from tests import general_test
from tests import multiGeneration_test
from tests import otherExporter_test
from tests import pyGeometry_test

SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt

class PyPRT_TestResult(unittest.TextTestResult):
    def startTestRun(self):
        pyprt.initialize_prt(SDK_PATH)

    def stopTestRun(self):
        pyprt.shutdown_prt()
        print("PRT is shut down.")


class PyPRT_TestRunner(unittest.TextTestRunner):
    def _makeResult(self):
        return PyPRT_TestResult(self.stream, self.descriptions, self.verbosity)


def testSuite():
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    suite.addTests(loader.loadTestsFromModule(general_test))
    suite.addTests(loader.loadTestsFromModule(multiGeneration_test))
    suite.addTests(loader.loadTestsFromModule(otherExporter_test))
    suite.addTests(loader.loadTestsFromModule(pyGeometry_test))
    return suite


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.join(os.path.abspath(sourcedir), 'src', 'pyprt')


class InstallCMakeLibsData(install_data):
    def run(self):
        self.outfiles = self.distribution.data_files


class InstallCMakeLibs(install_lib):
    def run(self):
        self.announce("Moving library files", level=3)
        self.skip_build = True

        build_bin_dir = os.path.join(self.distribution.bin_dir, 'bin')
        build_lib_dir = os.path.join(self.distribution.bin_dir, 'lib')
        os.makedirs(build_bin_dir, exist_ok=True)
        os.makedirs(build_lib_dir, exist_ok=True)

        install_dir = os.path.join(os.getcwd(), 'install')
        bin_dir = os.path.join(install_dir, 'bin')
        lib_dir = os.path.join(install_dir, 'lib')

        libs_bin = [os.path.join(bin_dir, _lib) for _lib in 
                    os.listdir(bin_dir) if 
                    os.path.isfile(os.path.join(bin_dir, _lib))]

        libs_lib = [os.path.join(lib_dir, _lib) for _lib in 
                    os.listdir(lib_dir) if 
                    os.path.isfile(os.path.join(lib_dir, _lib))]

        self.distribution.data_files = []

        for l in libs_bin:
            shutil.copyfile(l, os.path.join(build_bin_dir, os.path.basename(l)))
            self.distribution.data_files.append(os.path.join(build_bin_dir, os.path.basename(l)))

        for l in libs_lib:
            shutil.copyfile(l, os.path.join(build_lib_dir, os.path.basename(l)))
            self.distribution.data_files.append(os.path.join(build_lib_dir, os.path.basename(l)))

        self.distribution.run_command("install_data")
        super().run()


class InstallCMakeScripts(install_scripts):
    def run(self):
        self.announce("Moving scripts files", level=3)
        self.skip_build = True
        scripts_dir = os.path.join(os.getcwd(), 'install', 'scripts')
        os.makedirs(scripts_dir, exist_ok=True)

        for script in self.distribution.scripts:
            shutil.copyfile(os.path.join(os.getcwd(), os.path.dirname(script), os.path.basename(script)), os.path.join(scripts_dir, os.path.basename(script)))

        super().run()


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_cmake(ext)

    def build_cmake(self, extension: Extension):
        """
        The steps required to build the extension
        """

        self.announce("Preparing the build environment", level=3)

        build_dir = os.path.join(pathlib.Path(self.build_temp),'..','..')
        extension_path = pathlib.Path(self.get_ext_fullpath(extension.name))

        os.makedirs(pathlib.Path(self.build_temp), exist_ok=True)
        os.makedirs(extension_path.parent.absolute(), exist_ok=True)

        self.announce("Configuring cmake project", level=3)

        self.spawn(['cmake', '-H'+extension.sourcedir, '-B'+self.build_temp,
                    '-A'+'x64','-DPYTHON_EXECUTABLE='+sys.executable,
                    '-DCMAKE_BUILD_TYPE="RelWithDebInfo"'])

        self.announce("Building binaries", level=3)

        self.spawn(["cmake", "--build", self.build_temp, "--target", "INSTALL",
                    "--config", "RelWithDebInfo"])

        self.distribution.bin_dir = extension_path.parent.absolute()


class CustomTest(test):
    def run_tests(self):
        runner = PyPRT_TestRunner(verbosity=3)
        result = runner.run(testSuite())



setup(
    name='PyPRT',
    version='0.1.40',
    author='Camille Lechot',
    author_email='clechot@esri.com',
    description='Python bindings for CityEngine Procedural Runtime',
    long_description='',
    url='https://devtopia.esri.com/cami9495/py4prt',
    packages=find_packages('src'),
    package_dir={'':'src'},
    scripts=['scripts/utility.py'],
    include_package_data = True,
    ext_modules=[CMakeExtension('pyprt')],
    cmdclass={
        'build_ext' : CMakeBuild,
        'install_data' : InstallCMakeLibsData,
        'install_lib' : InstallCMakeLibs,
        'install_scripts' : InstallCMakeScripts,
        'test' : CustomTest},
    zip_safe=False,
    python_requires='>=3.6',
)