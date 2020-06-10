# Copyright (c) 2012-2020 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.

import unittest

import pyprt

import multiGeneration_test
import otherExporter_test
import pyGeometry_test
import shapeAttributesDict_test
import arcgis_test
import inspectRPK_test


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
    suite.addTests(loader.loadTestsFromModule(inspectRPK_test))
    return suite


def run_tests():
    runner = PyPRTTestRunner(verbosity=3)
    runner.run(test_suite())


if __name__ == '__main__':
    run_tests()
