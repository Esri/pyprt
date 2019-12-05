import os
import sys
import unittest

# current test workflow expects the developer to run 'setup.py bdist' first,
# so we append to the python module path
pyprt_build_dir = 'lib.win-amd64-3.6' if sys.platform.startswith('win32') else 'lib.linux-x86_64-3.6'
SDK_PATH = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'build', pyprt_build_dir)
sys.path.append(SDK_PATH)

import pyprt

import multiGeneration_test
import otherExporter_test
import pyGeometry_test
import shapeAttributesDict_test


class PyPRTTestResult(unittest.TextTestResult):
    def startTestRun(self):
        pyprt.initialize_prt()

    def stopTestRun(self):
        pyprt.shutdown_prt()
        print('PRT is shut down.')


class PyPRTTestRunner(unittest.TextTestRunner):
    def _makeResult(self):
        return PyPRTTestResult(self.stream, self.descriptions, self.verbosity)


def test_suite():
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    suite.addTests(loader.loadTestsFromModule(multiGeneration_test))
    suite.addTests(loader.loadTestsFromModule(otherExporter_test))
    suite.addTests(loader.loadTestsFromModule(pyGeometry_test))
    suite.addTests(loader.loadTestsFromModule(shapeAttributesDict_test))
    return suite


def run_tests():
    runner = PyPRTTestRunner(verbosity=3)
    runner.run(test_suite())


if __name__ == '__main__':
    run_tests()
