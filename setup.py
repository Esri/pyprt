import os
import re
import sys
import sysconfig
import platform
import subprocess

from distutils.command.install_data import install_data
from setuptools import setup, Extension, find_packages #, Distribution
from setuptools.command.build_ext import build_ext
from setuptools.command.install_lib import install_lib
from setuptools.command.install_scripts import install_scripts
import pathlib
import shutil


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.join(os.path.abspath(sourcedir), 'src')
        print("------SOURCE DIR: "+str(self.sourcedir))



class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        build_dir = pathlib.Path(self.build_temp)

        print("---EXT: "+str(extdir)) #C:\Users\cami9495\Documents\esri-cityengine-sdk-master\examples\py4prt\build\lib.win-amd64-3.6
        print("---BUILD: "+str(build_dir)) #build\temp.win-amd64-3.6\Release

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'RelWithDebInfo' #'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]
        #build_args = []

        if platform.system() == "Windows":
            # cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
            #     cfg.upper(),
            #     extdir)]
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)
        print()  # Add an empty line for cleaner output


class InstallCMakeLibsData(install_data):
    def run(self):
        self.outfiles = self.distribution.data_files

    
class InstallCMakeLibs(install_lib):
    def run(self):
        self.announce("Moving library files", level=3)
        self.skip_build = True
        self.install_dir = os.path.join(os.getcwd(), "build", "install")
        os.makedirs(self.install_dir, exist_ok=True)
        bin_dir = os.path.join(os.getcwd(), "build", "install", "bin")
        os.makedirs(bin_dir, exist_ok=True)
        lib_dir = os.path.join(os.getcwd(), "build", "install", "lib")
        os.makedirs(lib_dir, exist_ok=True)

        print("-----INSTALL:" + str(self.install_dir))
        print("---- BIN: "+ str(bin_dir))
        print("---BUILD:" + str(self.build_dir))
        print("---LIB:" + str(lib_dir))

        print(self.distribution.data_files)

        src1 = os.path.join(os.getcwd(), "build","temp.win-amd64-3.6","Release","codec","RelWithDebInfo")
        src2 = os.path.join(os.getcwd(), "build","temp.win-amd64-3.6","Release","_deps","prt-src","bin")
        src3 = os.path.join(os.getcwd(), "build","temp.win-amd64-3.6","Release","_deps","prt-src","lib")
        src4 = os.path.join(os.getcwd(), "build","lib.win-amd64-3.6","RelWithDebInfo")

        libs = [os.path.join(src1, "pyprt_codec.dll")]

        for item in os.listdir(src2):
            shutil.copyfile(os.path.join(src2, item), os.path.join(bin_dir, item))

        for item in os.listdir(src3):
            shutil.copyfile(os.path.join(src3, item), os.path.join(lib_dir, item))

        for lib in libs:
            shutil.copyfile(lib, os.path.join(lib_dir, os.path.basename(lib)))

        #self.distribution.data_files = [os.path.join(self.install_dir, os.path.basename(lib)) for lib in libs] # TO DO

        super().run()


class InstallCMakeScripts(install_scripts):
    def run(self):
        # self.announce("Moving scripts files", level=3)
        # self.skip_build = True

        # bin_dir = self.distribution.bin_dir

        # scripts_dirs = [os.path.join(bin_dir, _dir) for _dir in
        #                 os.listdir(bin_dir) if
        #                 os.path.isdir(os.path.join(bin_dir, _dir))]

        # for scripts_dir in scripts_dirs:

        #     shutil.move(scripts_dir,
        #                 os.path.join(self.build_dir,
        #                              os.path.basename(scripts_dir)))

        # Mark the scripts for installation, adding them to 
        # distribution.scripts seems to ensure that the setuptools' record 
        # writer appends them to installed-files.txt in the package's egg-info

        # self.distribution.scripts = scripts_dirs

        # super().run()



# class BinaryDistribution(Distribution):
#     def has_ext_modules(foo):
#         return True

setup(
    name='PyPRT',
    version='0.1.40',
    author='Camille Lechot',
    author_email='clechot@esri.com',
    description='Python bindings for CityEngine Procedural Runtime',
    long_description='',
    packages=find_packages('src'),
    package_dir={'':'src'},
    scripts=['src/utility.py'],
    #package_dir={'pyprt': 'install'},
    # package_data={
    #     'pyprt' : [
    #         #'bin/pyprt.cp36-win_amd64.pyd',
    #         'glutess.dll',
    #         'glutess.lib',
    #         'com.esri.prt.core.dll',
    #         'com.esri.prt.core.lib',
    #         'com.esri.prt.adaptors.dll',
    #         'com.esri.prt.alembic.dll',
    #         'com.esri.prt.codecs.dll',
    #         'pyprt_codec.dll'
    #     ]
    # },
    # distclass=BinaryDistribution,
    ext_modules=[CMakeExtension('pyprt')],
    cmdclass={
        'build_ext' : CMakeBuild,
        'install_data' : InstallCMakeLibsData,
        'install_lib' : InstallCMakeLibs},
    #'install' : InstallCommand},
    test_suite='tests.runner',
    zip_safe=False
)