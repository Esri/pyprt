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


class MultiTest(unittest.TestCase):
    def test_multiGenerations(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        m = pyprt.ModelGenerator([shape_geo])
        model1 = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        model2 = m.generate_model([attrs])
        model3 = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder',
                                  {'emitReports': True, 'emitGeometry': False})
        model4 = m.generate_model([attrs])
        self.assertDictEqual(model1[0].get_report(), model2[0].get_report())
        self.assertDictEqual(model1[0].get_report(), model3[0].get_report())
        self.assertListEqual(model1[0].get_vertices(),
                             model2[0].get_vertices())
        self.assertListEqual(model3[0].get_vertices(),
                             model4[0].get_vertices())

    def test_PathAndGeometryInitShapes(self):
        rpk = asset_file('extrusion_rule.rpk')
        attrs = {}
        shape_geo = pyprt.InitialShape(
            [-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        m = pyprt.ModelGenerator([shape_geo, shape_geo_from_obj])
        model = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
        modelb = m.generate_model([attrs])
        self.assertEqual(model[0].get_report(), modelb[0].get_report())
        self.assertEqual(model[1].get_report(), modelb[1].get_report())
        self.assertListEqual(model[0].get_vertices(), modelb[0].get_vertices())
        self.assertListEqual(model[1].get_vertices(), modelb[1].get_vertices())
