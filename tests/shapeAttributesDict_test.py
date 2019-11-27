import unittest
import os
import sys

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'data', filename)


class ShapeAttributesTest(unittest.TestCase):
    def test_correctExecution(self):
        rpk = asset_file('test_rule.rpk')
        attrs_1 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint'}
        attrs_2 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
        attrs_3 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint', 'text': 'hello'}

        shape_geometry_1 = pyprt.Geometry(
            [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
        shape_geometry_2 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  10, 0, -10,  10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs_1, attrs_2, attrs_3], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model), 3)

    def test_oneDictForAll(self):
        rpk = asset_file('test_rule.rpk')
        attrs_1 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint'}
        attrs_2 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
        attrs_3 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint', 'text': 'hello'}
        shape_geometry_1 = pyprt.Geometry(
            [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
        shape_geometry_2 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  10, 0, -10,  10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs_3], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model), 3)

    def test_wrongNumberOfDict(self):
        rpk = asset_file('test_rule.rpk')
        attrs_1 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint'}
        attrs_2 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
        attrs_3 = {'ruleFile': 'bin/test_rule.cgb', 'startRule': 'Default$Footprint', 'text': 'hello'}
        shape_geometry_1 = pyprt.Geometry(
            [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
        shape_geometry_2 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  10, 0, -10,  10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs_1, attrs_2], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model), 0)