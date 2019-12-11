import os
import sys
import unittest

from pyprt import pyprt

import multiGeneration_test
import otherExporter_test
import pyGeometry_test
import shapeAttributesDict_test
import arcgis_test


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
    suite.addTests(loader.loadTestsFromModule(arcgis_test))
    return suite


def run_tests():
    runner = PyPRTTestRunner(verbosity=3)
    runner.run(test_suite())


if __name__ == '__main__':
    run_tests()
