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
import pathlib
import shutil


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.join(os.path.abspath(sourcedir), 'src') #C:\Users\cami9495\Documents\esri-cityengine-sdk-master\examples\py4prt\src


class InstallCMakeLibsData(install_data):
    def run(self):
        self.outfiles = self.distribution.data_files


class InstallCMakeLibs(install_lib):
    def run(self):
        self.announce("Moving library files", level=3)
        self.skip_build = True

        build_bin_dir = self.distribution.bin_dir
        build_lib_dir = os.path.join(build_bin_dir, '..', 'lib.win-amd64-3.6')

        install_dir = os.path.join(os.getcwd(), "install")
        bin_dir = os.path.join(install_dir, "bin")
        lib_dir = os.path.join(install_dir, "lib")

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


# class InstallCMakeScripts(install_scripts):
#     def run(self):
#         self.announce("Moving scripts files", level=3)
#         self.skip_build = True

#         super().run()



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
        print()

    def build_cmake(self, extension: Extension):
        """
        The steps required to build the extension
        """

        self.announce("Preparing the build environment", level=3)

        build_dir = os.path.join(pathlib.Path(self.build_temp),'..','..') #build\temp.win-amd64-3.6\Release
        extension_path = pathlib.Path(self.get_ext_fullpath(extension.name)) #build\lib.win-amd64-3.6\pyprt.cp36-win_amd64.pyd

        os.makedirs(pathlib.Path(self.build_temp), exist_ok=True)
        os.makedirs(extension_path.parent.absolute(), exist_ok=True)

        self.announce("Configuring cmake project", level=3)

        self.spawn(['cmake', '-H'+extension.sourcedir, '-B'+self.build_temp,
                    '-A'+'x64','-DPYTHON_EXECUTABLE='+sys.executable,
                    '-DCMAKE_BUILD_TYPE="RelWithDebInfo"'])

        self.announce("Building binaries", level=3)

        self.spawn(["cmake", "--build", self.build_temp, "--target", "INSTALL",
                    "--config", "RelWithDebInfo"])

        # self.announce("Moving built python module", level=3)

        build_bin_dir = os.path.join(build_dir, 'bin.win-amd64-3.6') #TO IMPROVE
        os.makedirs(build_bin_dir, exist_ok=True)
        self.distribution.bin_dir = build_bin_dir #BEFORE: build\temp.win-amd64-3.6\Release\bin\Release
        # self.distribution.bin_dir = extension_path.parent.absolute()

        # pyd_path = [os.path.join(bin_dir, _pyd) for _pyd in
        #             os.listdir(bin_dir) if
        #             os.path.isfile(os.path.join(bin_dir, _pyd)) and
        #             os.path.splitext(_pyd)[0].startswith(PACKAGE_NAME) and
        #             os.path.splitext(_pyd)[1] in [".pyd", ".so"]][0]

        # print(pyd_path)

        # shutil.copy(pyd_path, extension_path)



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
    url='https://devtopia.esri.com/cami9495/py4prt',
    packages=find_packages('src'),
    package_dir={'':'src'},
    scripts=['src/utility.py'],
    # package_data={
    #     'pyprt' : [
    #         'install/bin/glutess.dll',
    #         'install/bin/glutess.lib',
    #         'install/bin/com.esri.prt.core.dll',
    #         'install/bin/com.esri.prt.core.lib',
    #         'install/lib/com.esri.prt.adaptors.dll',
    #         'install/lib/com.esri.prt.alembic.dll',
    #         'install/lib/com.esri.prt.codecs.dll',
    #         'install/lib/pyprt_codec.dll'
    #     ]
    # },
    ext_modules=[CMakeExtension('pyprt')],
    cmdclass={
        'build_ext' : CMakeBuild,
        'install_data' : InstallCMakeLibsData,
        'install_lib' : InstallCMakeLibs},
        #'install_scripts' : InstallCMakeScripts},
    test_suite='tests.runner',
    zip_safe=False
)