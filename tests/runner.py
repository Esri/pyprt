import unittest
import os
import sys

import general_test
import multiGeneration_test
import otherExporter_test
import pyGeometry_test

SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt

def setUpPackage():
    print("SETUP CALLED!!!!")
    pyprt.initialize_prt(SDK_PATH)

def tearDownPackage():
    print("TEARDOWN CALLED!!!!")
    pyprt.shutdown_prt()

# initialize the test suite
loader = unittest.TestLoader()
suite  = unittest.TestSuite()

# add tests to the test suite
suite.addTests(loader.loadTestsFromModule(general_test))
suite.addTests(loader.loadTestsFromModule(multiGeneration_test))
suite.addTests(loader.loadTestsFromModule(otherExporter_test))
suite.addTests(loader.loadTestsFromModule(pyGeometry_test))

# initialize a runner, pass it your suite and run it
runner = unittest.TextTestRunner(verbosity=3)
setUpPackage()
result = runner.run(suite)
tearDownPackage()