import unittest
import os
import sys

SDK_PATH = os.path.join(os.getcwd(), 'build', 'lib.win-amd64-3.6', 'PyPRT', 'pyprt', 'bin')
sys.path.append(SDK_PATH)

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'data', filename)


class ShapeAttributesTest(unittest.TestCase):
    def test_correct_execution(self):
        rpk = asset_file('candler.rpk')
        attrs = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 555}
        attrs_2 = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 111}
        attrs_3 = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 333}
        shape_geometry_1 = pyprt.Geometry(
            [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
        shape_geometry_2 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  10, 0, -10,  10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs, attrs_2, attrs_3], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport' : False, 'emitGeometry' : True})
        self.assertEqual(len(model), 3)

    def test_one_dict_for_all(self):
        rpk = asset_file('candler.rpk')
        attrs = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 555}
        attrs_2 = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 111}
        attrs_3 = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 333}
        shape_geometry_1 = pyprt.Geometry(
            [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
        shape_geometry_2 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  10, 0, -10,  10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport' : False, 'emitGeometry' : True})
        self.assertEqual(len(model), 3)

    def test_wrong_number_of_dict(self):
        rpk = asset_file('candler.rpk')
        attrs = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 555}
        attrs_2 = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 111}
        attrs_3 = {'ruleFile' : 'bin/candler.cgb', 'startRule' : 'Default$Footprint', 'seed': 333}
        shape_geometry_1 = pyprt.Geometry(
            [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
        shape_geometry_2 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])
        shape_geometry_3 = pyprt.Geometry(
            [0, 0, 0,  0, 0, -10,  10, 0, -10,  10, 0, 0, -5, 0, -5])
        m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2, shape_geometry_3])
        model = m.generate_model([attrs, attrs_2], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport' : False, 'emitGeometry' : True})
        self.assertEqual(len(model), 0)