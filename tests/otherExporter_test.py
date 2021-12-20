# Copyright (c) 2012-2022 Esri R&D Center Zurich

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


class ExporterTest(unittest.TestCase):
    def test_slpk(self):
        if os.path.isfile(asset_output_file('Unittest4SLPK.slpk')):
            os.remove(asset_output_file('Unittest4SLPK.slpk'))

        encoder_options = {
            'outputPath': os.path.dirname(asset_output_file(''))}
        os.makedirs(encoder_options['outputPath'], exist_ok=True)

        shape_geo_from_obj = pyprt.InitialShape(
            asset_file('building_parcel.obj'))
        rpk = asset_file('envelope2002.rpk')
        attrs = {'report_but_not_display_green': True}
        slpk_options = {'layerTextureEncoding': ['2'], 'layerEnabled': [True], 'layerUID': ['1'],
                        'layerName': ['Salut'], 'layerTextureQuality': [1.0], 'layerTextureCompression': [9],
                        'layerTextureScaling': [1.0], 'layerTextureMaxDimension': [2048],
                        'layerFeatureGranularity': ['0'], 'layerBackfaceCulling': [False], 'baseName': 'Unittest4SLPK'}
        slpk_options.update(encoder_options)
        m = pyprt.ModelGenerator([shape_geo_from_obj])
        m.generate_model(
            [attrs], rpk, 'com.esri.prt.codecs.I3SEncoder', slpk_options)
        self.assertTrue(os.path.isfile(
            asset_output_file('Unittest4SLPK.slpk')))
        self.assertGreater(
            os.stat(asset_output_file('CGAReport.txt')).st_size, 0)
