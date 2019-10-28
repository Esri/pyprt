import os
import sys
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