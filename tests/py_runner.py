import os
import sys
import unittest

import general_test
import multiGeneration_test
import otherExporter_test
import pyGeometry_test

SDK_PATH = os.path.join(os.getcwd(), 'build', 'lib.win-amd64-3.6', 'PyPRT', 'pyprt', 'bin')
sys.path.append(SDK_PATH)

import pyprt


class PyPRT_TestResult(unittest.TextTestResult):
    def startTestRun(self):
        pyprt.initialize_prt()

    def stopTestRun(self):
        pyprt.shutdown_prt()
        print('PRT is shut down.')


class PyPRT_TestRunner(unittest.TextTestRunner):
    def _makeResult(self):
        return PyPRT_TestResult(self.stream, self.descriptions, self.verbosity)


def test_suite():
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    suite.addTests(loader.loadTestsFromModule(general_test))
    suite.addTests(loader.loadTestsFromModule(multiGeneration_test))
    suite.addTests(loader.loadTestsFromModule(otherExporter_test))
    suite.addTests(loader.loadTestsFromModule(pyGeometry_test))
    return suite


def run_tests():
    runner = PyPRT_TestRunner(verbosity=3)
    result = runner.run(test_suite())


if __name__ == '__main__':
    run_tests()