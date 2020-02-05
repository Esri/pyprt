import os
import unittest

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'data', filename)


class MultiTest(unittest.TestCase):
    def test_multiGenerations(self):
        rpk = asset_file('simple_rule0819.rpk')
        attrs = {'ruleFile': 'bin/simple_rule2019.cgb', 'startRule': 'Default$Footprint'}
        shape_geo = pyprt.InitialShape([-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        m = pyprt.ModelGenerator([shape_geo])
        model1 = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        model2 = m.generate_model([attrs])
        model3 = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder',
                                  {'emitReports': True, 'emitGeometry': False})
        model4 = m.generate_model([attrs])
        self.assertDictEqual(model1[0].get_report(), model2[0].get_report())
        self.assertDictEqual(model1[0].get_report(), model3[0].get_report())
        self.assertListEqual(model1[0].get_vertices(), model2[0].get_vertices())
        self.assertListEqual(model3[0].get_vertices(), model4[0].get_vertices())

    def test_PathAndGeometryInitShapes(self):
        rpk = asset_file('simple_rule0819.rpk')
        attrs = {'ruleFile': 'bin/simple_rule2019.cgb', 'startRule': 'Default$Footprint'}
        shape_geo = pyprt.InitialShape([-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        shape_geo_from_obj = pyprt.InitialShape(asset_file('bigFootprint_0.obj'))
        m = pyprt.ModelGenerator([shape_geo, shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        modelb = m.generate_model([attrs])
        self.assertEqual(model[0].get_report(), modelb[0].get_report())
        self.assertEqual(model[1].get_report(), modelb[1].get_report())
        self.assertListEqual(model[0].get_vertices(), modelb[0].get_vertices())
        self.assertListEqual(model[1].get_vertices(), modelb[1].get_vertices())
