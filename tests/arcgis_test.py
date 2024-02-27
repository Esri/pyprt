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

import os
import unittest

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


class ArcGISAPITest(unittest.TestCase):
    def test_import(self):

        from arcgis.gis import GIS
        from pyprt.pyprt_arcgis import arcgis_to_pyprt

        gis = GIS()
        item = gis.content.get('6ddd4741514d4e47b005c4962f06de58')
        fset = item.layers[0].query(return_z=True)

        initial_geometries = arcgis_to_pyprt(fset)

        self.assertEqual(len(initial_geometries), 2)

    def test_arcgis_with_holes(self):
        
        from arcgis.gis import GIS
        from pyprt.pyprt_arcgis import arcgis_to_pyprt

        gis = GIS()
        shapes_id = 'd9e727c94cf041d1a353cd43b1c05d0e'
        item = gis.content.get(shapes_id)
        shapes_set = item.layers[0].query(return_z=True)

        initial_geometries_from_set = arcgis_to_pyprt(shapes_set)

        rpk = asset_file('FacesHolesVerticesrule.rpk')
        attrs = {}

        m = pyprt.ModelGenerator(initial_geometries_from_set)
        models = m.generate_model(
            [attrs], rpk, 'com.esri.pyprt.PyEncoder', {})

        expected_content = ("14\n"
                            "9\n"
                            "2\n"
                            "14\n"
                            "9\n"
                            "0\n"
                            "16\n"
                            "10\n"
                            "0\n"
                            "26\n"
                            "15\n"
                            "4\n")
            
        actual_content = ""
        for model in models:
            actual_content += model.get_cga_prints()

        self.assertEqual(expected_content,actual_content)
