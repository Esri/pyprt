from setuptools import setup, find_packages

setup(
    name='PyPRT',
    version='0.1',
    author='Camille Lechot',
    author_email='clechot@esri.com',
    description='Python bindings for CityEngine Procedural Runtime',
    long_description='',
    # packages=find_packages('src'),
    # package_dir={'':'src'},
    ext_modules=[],
    test_suite='tests.runner',
    zip_safe=False,
)