# Copyright (c) 2012-2023 Esri R&D Center Zurich

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


class InspectRPKTest(unittest.TestCase):
    def test_candler_rpk(self):
        rpk = asset_file('candler.rpk')

        inspect_dict = pyprt.get_rpk_attributes_info(rpk)
        ground_truth_keys = ['BuildingHeight', 'ColorizeWall', 'CornerWallWidth', 'CorniceOverhang', 'FloorHeight', 'FrontWindowWidth',
                             'GroundfloorHeight', 'Mode', 'RearWindowWidth', 'SillSize', 'TileWidth', 'WallTexture', 'WindowHeight']

        self.assertSetEqual(set(inspect_dict.keys()), set(ground_truth_keys))

    def test_candler_rpk_attr(self):
        rpk = asset_file('candler.rpk')

        inspect_dict = pyprt.get_rpk_attributes_info(rpk)
        ground_truth_dict = {'annotations': [['@Order', ['#NULL#', 3.0]],['@Range',['#NULL#', 0.5],['#NULL#', 2.0]],['@Group',['#NULL#', 'Windows'],['#NULL#', 4.0]]],
                             'type': 'float'}

        self.assertDictEqual(inspect_dict['RearWindowWidth'], ground_truth_dict)
