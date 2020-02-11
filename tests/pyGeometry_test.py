import os
import unittest

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


class GeometryTest(unittest.TestCase):
    def test_verticesNber_candler(self):
        rpk = asset_file('candler.rpk')
        attrs = {'ruleFile': 'bin/candler.cgb',
                 'startRule': 'Default$Footprint'}
        shape_geo_from_obj = asset_file('candler_footprint.obj')
        m = pyprt.ModelGenerator(shape_geo_from_obj)
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model[0].get_vertices()), 97050)

    def test_facesNber_candler(self):
        rpk = asset_file('candler.rpk')
        attrs = {'ruleFile': 'bin/candler.cgb',
                 'startRule': 'Default$Footprint'}
        shape_geo_from_obj = asset_file('candler_footprint.obj')
        m = pyprt.ModelGenerator(shape_geo_from_obj)
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model[0].get_faces()), 47202)

    def test_report_green(self):
        rpk = asset_file('envelope0110.rpk')
        attrs = {'ruleFile': 'rules/typology/envelope0110.cgb', 'startRule': 'Default$Lot',
                 'report_but_not_display_green': True}
        shape_geo_from_obj = asset_file('building_parcel.obj')
        m = pyprt.ModelGenerator(shape_geo_from_obj)
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': True, 'emitGeometry': False})
        ground_truth_dict = {'Floor area_n': 20.0, 'Greenery Area_n': 574.0, 'Number of trees_n': 114.0, 'green area_n': 460.0, 'total report for optimisation_n': 20.0, 'Floor area_sum': 10099.37, 'Floor area_avg': 504.97, 'Greenery Area_sum': 2123.40, 'Greenery Area_avg': 3.70, 'Number of trees_sum': 114.0, 'Number of trees_avg': 1.0, 'green area_sum': 1553.40, 'green area_avg': 3.38, 'total report for optimisation_sum': 807.95,
                             'total report for optimisation_avg': 40.40, 'Floor area_min': 294.37, 'Floor area_max': 874.87, 'Greenery Area_min': 0.49, 'Greenery Area_max': 408.27, 'Number of trees_min': 1.0, 'Number of trees_max': 1.0, 'green area_min': 0.49, 'green area_max': 408.27, 'total report for optimisation_min': 23.55, 'total report for optimisation_max': 69.99}
        rep = model[0].get_report()
        rep_round = {x: round(z, 2) for x, z in rep.items()}
        self.assertEqual(rep_round, ground_truth_dict)

    def test_noReport(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {'ruleFile': 'bin/extrusion_rule.cgb',
                 'startRule': 'Default$Footprint'}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 5.0, -5.0, 0.0, 6.0, 20.0, 0.0, 5.0, 15.0, 0.0, 3.0])
        m = pyprt.ModelGenerator([shape_geo])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False})
        self.assertDictEqual(model[0].get_report(), {})

    def test_noGeometry(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {'ruleFile': 'bin/extrusion_rule.cgb',
                 'startRule': 'Default$Footprint'}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 5.0, -5.0, 0.0, 6.0, 20.0, 0.0, 5.0, 15.0, 0.0, 3.0])
        m = pyprt.ModelGenerator([shape_geo])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitGeometry': False})
        self.assertListEqual(model[0].get_vertices(), [])

    def test_buildingHeight(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {'ruleFile': 'bin/extrusion_rule.cgb',
                 'startRule': 'Default$Footprint'}
        attrs2 = {'ruleFile': 'bin/extrusion_rule.cgb', 'startRule': 'Default$Footprint', 'minBuildingHeight': 23.0,
                  'maxBuildingHeight': 23.0}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        m = pyprt.ModelGenerator([shape_geo])
        m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        model2 = m.generate_model([attrs2])
        z_coord = [round(b, 1) for a, b, c in model2[0].get_vertices()]
        for z in z_coord:
            if z:
                self.assertAlmostEqual(abs(z), 23.0)
