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

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


def asset_output_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'output', filename)


class GeometryTest(unittest.TestCase):
    def test_verticesnber_candler(self):
        rpk = asset_file('candler.rpk')
        attrs = {}
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('candler_footprint.obj'))
        m = pyprt.ModelGenerator([shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model[0].get_vertices()), 97050*3)

    def test_facesnber_candler(self):
        rpk = asset_file('candler.rpk')
        attrs = {}
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('candler_footprint.obj'))
        m = pyprt.ModelGenerator([shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False, 'emitGeometry': True})
        self.assertEqual(len(model[0].get_faces()), 47202)

    def test_report_green(self):
        rpk = asset_file('envelope2002.rpk')
        attrs = {'report_but_not_display_green': True, 'seed': 666}
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        m = pyprt.ModelGenerator([shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': True, 'emitGeometry': False})
        ground_truth_dict = {'Floor area_n': 20.0, 'Greenery Area_n': 574.0, 'Number of trees_n': 114.0, 'green area_n': 460.0, 'total report for optimisation_n': 20.0, 'Floor area_sum': 10099.37, 'Floor area_avg': 504.97, 'Greenery Area_sum': 2123.40, 'Greenery Area_avg': 3.70, 'Number of trees_sum': 114.0, 'Number of trees_avg': 1.0, 'green area_sum': 1553.40, 'green area_avg': 3.38, 'total report for optimisation_sum': 807.95,
                             'total report for optimisation_avg': 40.40, 'Floor area_min': 294.37, 'Floor area_max': 874.87, 'Greenery Area_min': 0.49, 'Greenery Area_max': 408.27, 'Number of trees_min': 1.0, 'Number of trees_max': 1.0, 'green area_min': 0.49, 'green area_max': 408.27, 'total report for optimisation_min': 23.55, 'total report for optimisation_max': 69.99}
        rep = model[0].get_report()
        rep_round = {x: round(z, 2) for x, z in rep.items()}
        self.assertEqual(rep_round, ground_truth_dict)

    def test_noreport(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 5.0, -5.0, 0.0, 6.0, 20.0, 0.0, 5.0, 15.0, 0.0, 3.0])
        m = pyprt.ModelGenerator([shape_geo])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False})
        self.assertDictEqual(model[0].get_report(), {})

    def test_nogeometry(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 5.0, -5.0, 0.0, 6.0, 20.0, 0.0, 5.0, 15.0, 0.0, 3.0])
        m = pyprt.ModelGenerator([shape_geo])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitGeometry': False})
        self.assertListEqual(model[0].get_vertices(), [])

    def test_buildingheight(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {}
        attrs2 = {'minBuildingHeight': 23.0,
                  'maxBuildingHeight': 23.0}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        m = pyprt.ModelGenerator([shape_geo])
        m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        model2 = m.generate_model([attrs2], rpk, 'com.esri.pyprt.PyEncoder', {})
        z_coord = [round(b, 1) for b in model2[0].get_vertices()[1:-1:3]]
        for z in z_coord:
            if z:
                self.assertAlmostEqual(abs(z), 23.0)

    def test_faces_data(self):
        rpk = asset_file('candler.rpk')
        attrs = {}
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('candler_footprint.obj'))
        m = pyprt.ModelGenerator([shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                                 'emitReport': False})
        cnt = 0
        for f in model[0].get_faces():
            cnt += f
        self.assertEqual(cnt, len(model[0].get_indices()))

    def test_path_geometry_initshapes(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        m1 = pyprt.ModelGenerator([shape_geo])
        m2 = pyprt.ModelGenerator([shape_geo_from_obj])
        m3 = pyprt.ModelGenerator([shape_geo, shape_geo_from_obj])
        model1 = m1.generate_model(
            [attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        model2 = m2.generate_model(
            [attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        model3 = m3.generate_model(
            [attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        self.assertEqual(model1[0].get_report(), model3[0].get_report())
        self.assertEqual(model2[0].get_report(), model3[1].get_report())
        self.assertListEqual(model1[0].get_vertices(),
                             model3[0].get_vertices())
        self.assertListEqual(model2[0].get_vertices(),
                             model3[1].get_vertices())

    def test_initial_shape_with_hole(self):
        rpk = asset_file('FacesHolesVerticesrule.rpk')
        attrs = {}
        shape_with_hole = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0, 2, 0, 2, 8, 0, 8, 2, 0, 8], [
                                             0, 1, 2, 3, 4, 5, 6], [4, 3], [[0, 1]])

        encoder_options = {
            'outputPath': os.path.dirname(asset_output_file(''))}
        os.makedirs(encoder_options['outputPath'], exist_ok=True)

        m = pyprt.ModelGenerator([shape_with_hole])
        m.generate_model(
            [attrs], rpk, 'com.esri.prt.codecs.OBJEncoder', encoder_options)

        expected_file = os.path.join(
            encoder_options['outputPath'], 'CGAPrint.txt')
        expected_content = ("14\n"
                            "9\n"
                            "2\n")

        self.assertTrue(os.path.exists(expected_file))
        with open(expected_file, 'r') as cga_print_file:
            cga_print = cga_print_file.read()
            self.assertEqual(cga_print, expected_content)

    def test_cga_prints_green(self):
        rpk = asset_file('envelope2002.rpk')
        attrs = {'report_but_not_display_green': True, 'seed': 2}
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        m = pyprt.ModelGenerator([shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
            'emitReport': True, 'emitGeometry': False})

        self.assertEqual(model[0].get_cga_prints(), str(attrs['seed'])+"\n")

    def test_cga_errors_holes(self):
        rpk = asset_file('FacesHolesVerticesrule.rpk')
        attrs = {}
        shape_with_hole_with_error = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0, 2, 0, 2, 8, 0, 8, 2, 0, 8], [
                                             0, 1, 2, 3, 4, 5, 6], [4, 3], [[0, 1, 1]])
        m = pyprt.ModelGenerator([shape_with_hole_with_error])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
            'emitReport': True, 'emitGeometry': False})

        self.assertEqual(len(model[0].get_cga_errors()), 1)

    def test_attributesvalue_fct(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {'maxBuildingHeight':35.0}
        attrs2 = {}
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        m = pyprt.ModelGenerator([shape_geo_from_obj, shape_geo_from_obj])
        model = m.generate_model([attrs, attrs2], rpk, 'com.esri.pyprt.PyEncoder', {})
        self.assertDictEqual(model[0].get_attributes_values(),
                {'maxBuildingHeight': 35.0, 'OBJECTID': 0.0, 'minBuildingHeight': 10.0, 'buildingColor': '#FF00FF', 'text': 'salut'})
        self.assertDictEqual(model[1].get_attributes_values(),
                {'OBJECTID': 0.0, 'minBuildingHeight': 10.0, 'buildingColor': '#FF00FF', 'text': 'salut', 'maxBuildingHeight': 30.0})
