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

import os
import unittest
import tempfile

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


class ShapeAttributesTest(unittest.TestCase):

    def test_correctExecution(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs_1 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint'}
        attrs_2 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
        attrs_3 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint', 'text': 'hello'}

        shape_geometry_1 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
        shape_geometry_2 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, -10, -10, 0, -10, -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, -10, 10, 0, -10, 10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator(
            [shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs_1, attrs_2, attrs_3], rpk, 'com.esri.pyprt.PyEncoder',
                                 {'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model), 3)

    def test_oneDictForAll(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {'ruleFile': 'bin/extrusion_rule.cgb',
                 'startRule': 'Default$Footprint', 'text': 'hello'}
        shape_geometry_1 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
        shape_geometry_2 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, -10, -10, 0, -10, -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, -10, 10, 0, -10, 10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator(
            [shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder',
                                 {'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model), 3)

    def test_wrongNumberOfDict(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs_1 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint'}
        attrs_2 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
        shape_geometry_1 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
        shape_geometry_2 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, -10, -10, 0, -10, -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.InitialShape(
            [0, 0, 0, 0, 0, -10, 10, 0, -10, 10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator(
            [shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs_1, attrs_2], rpk, 'com.esri.pyprt.PyEncoder',
                                 {'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model), 0)

    def test_oneDictPerInitialShapeType(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs_1 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint'}
        attrs_2 = {'ruleFile': 'bin/extrusion_rule.cgb',
                   'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
        shape_geometry_1 = pyprt.InitialShape(
            [-7.666, 0.0, -0.203, -7.666, 0.0, 44.051, 32.557, 0.0, 44.051, 32.557, 0.0, -0.203])
        shape_geometry_2 = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2])
        model = m.generate_model(
            [attrs_1, attrs_2], rpk, 'com.esri.pyprt.PyEncoder', {})
        self.assertNotEqual(model[0].get_report()[
                            'Min Height.0_avg'], model[1].get_report()['Min Height.0_avg'])

    def test_arrayAttributes(self):
        with tempfile.TemporaryDirectory() as output_path:
            rpk = asset_file('arrayAttrs.rpk')
            attrs = {'ruleFile': 'bin/arrayAttrs.cgb',
                     'startRule': 'Default$Init',
                     'arrayAttrFloat': [0.0, 1.0, 2.0],
                     'arrayAttrBool': [False, False, True],
                     'arrayAttrString': ['foo', 'bar', 'baz']}
            initial_shape = pyprt.InitialShape(
                [-7.666, 0.0, -0.203, -7.666, 0.0, 44.051, 32.557, 0.0, 44.051, 32.557, 0.0, -0.203])
            m = pyprt.ModelGenerator([initial_shape])
            m.generate_model([attrs], rpk, 'com.esri.prt.codecs.OBJEncoder', {'outputPath': output_path})

            expected_file = os.path.join(output_path, 'CGAPrint.txt')
            expected_content = ("arrayAttrFloat = (3)[0,1,2]\n"
                                "arrayAttrBool = (3)[false,false,true]\n"
                                "arrayAttrString = (3)[foo,bar,baz]\n")

            self.assertTrue(os.path.exists(expected_file))
            with open(expected_file, 'r') as cga_print_file:
                cga_print = cga_print_file.read()
                self.assertEqual(cga_print, expected_content)
