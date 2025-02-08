# Copyright (c) 2012-2024 Esri R&D Center Zurich

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

import sys
import os
import unittest
import xmlrunner
import argparse

import pyprt

import otherExporter_test
import pyGeometry_test
import shapeAttributesDict_test
import arcgis_test
import inspectRPK_test
import custom_adaptor


def test_suite():
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    # suite.addTests(loader.loadTestsFromModule(otherExporter_test))
    # suite.addTests(loader.loadTestsFromModule(pyGeometry_test))
    # suite.addTests(loader.loadTestsFromModule(shapeAttributesDict_test))
    # suite.addTests(loader.loadTestsFromModule(inspectRPK_test))
    # suite.addTests(loader.loadTestsFromModule(arcgis_test))
    suite.addTests(loader.loadTestsFromModule(custom_adaptor))
    return suite


def run_tests(test_xml_reports_path = None):
    if test_xml_reports_path:
        runner = xmlrunner.XMLTestRunner(verbosity=3, output=test_xml_reports_path)
    else:
        runner = unittest.TextTestRunner(verbosity=3)

    runner.run(test_suite())


if __name__ == '__main__':    
    parser = argparse.ArgumentParser()
    parser.add_argument("--xml_output_directory", help="Output directory for XML test reports (must exist).", )
    args = parser.parse_args()

    xml_output_directory = None
    if args.xml_output_directory:
        xml_output_directory = os.path.realpath(args.xml_output_directory)
        if not (os.path.exists(xml_output_directory) and os.path.isdir(xml_output_directory)):
            raise "XML reports output directory does not exist: " + xml_output_directory
   
    run_tests(xml_output_directory)
