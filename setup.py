import os
import sys
import subprocess

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.file_util import copy_file
from distutils.dir_util import copy_tree


class CMakeConfig:
    def __init__(self):
        self.cmake_build_type = 'RelWithDebInfo'
        self.cmake_executable = self.detect_cmake()
        self.make_executable, self.cmake_generator = self.detect_make()

    def detect_cmake(self):
        cmake_home = os.getenv('CMAKE313_HOME', '')
        cmake_candidates = [
            [os.path.join(cmake_home, 'cmake'), '--version'],  # 1. try env var (typically for CI)
            ['cmake', '--version']                             # 2. try PATH (typically for devs)
        ]
        return self.try_alternatives('cmake', cmake_candidates)

    def detect_make(self):
        make_home = os.getenv('NINJA_HOME', '')
        make_candidates = [
            [os.path.join(make_home, 'ninja'), '--version'],  # 1. try env var
            ['ninja', '--version'],                           # 2. try PATH with ninja
            ['ninja-build', '--version'],                     # 3. try PATH with alternative name (e.g. used in CentOS)
            ['make', '--version'],                            # 4. try PATH with make (macos, linux)
            ['nmake', '/?']                                   # 5. try PATH with nmake (windows)
        ]
        make_executable = self.try_alternatives('ninja or (n)make', make_candidates)

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

        cmake_install_prefix = os.path.join(self.build_lib, 'pyprt')

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

        prt_dir = os.getenv('PRT_DIR', '')
        if prt_dir != '':
            cmake_configure_command.append('-Dprt_DIR={}'.format(prt_dir))

        self.spawn(cmake_configure_command)

        self.announce('Building binaries', level=3)
        cmake_build_command = [cmake.cmake_executable, '--build', self.build_temp]
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
            filename = self.get_ext_filename(fullname)
            modpath = fullname.split('.')
            package = '.'.join(modpath[:-1])
            package_dir = build_py.get_package_dir(package)
            dest_filename = os.path.join(package_dir,
                                         os.path.basename(filename))
            src_filename = os.path.join(self.build_lib, filename)

            # Always copy, even if source is older than destination, to ensure
            # that the right extensions for the current Python/platform are
            # used.
            copy_file(
                src_filename, dest_filename, verbose=self.verbose,
                dry_run=self.dry_run
            )
            copy_tree(self.build_lib, os.curdir,
                      verbose=self.verbose, dry_run=self.dry_run)
            if ext._needs_stub:
                self.write_stub(package_dir or os.curdir, ext, True)



cmake = CMakeConfig()
print(cmake)

setup(
    name='pyprt',
    version='0.3.0',  # keep consistent with __version__ in pyprt/__init__.py
    author='Camille Lechot',
    author_email='clechot@esri.com',
    description='Python bindings for the "Procedural Runtime SDK" (PRT) of "ArcGIS CityEngine" by Esri.',
    long_description='The goal of this project is to enable the execution of CityEngine rules within Python world. ',
    url='https://devtopia.esri.com/Zurich-R-D-Center/pyprt',
    platforms=['Windows', 'Linux'],
    packages=find_packages(exclude=['tests']),
    include_package_data=True,
    ext_modules=[CMakeExtension('pyprt.pyprt', 'src')], 
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
    python_requires='>=3.6'
)
