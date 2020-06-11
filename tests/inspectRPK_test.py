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


class InspectRPKTest(unittest.TestCase):
    def test_candlerRPK(self):
        rpk = asset_file('candler.rpk')

        inspect_dict = pyprt.inspect_rpk(rpk)
        ground_truth_dict = {'BuildingHeight': 'float', 'ColorizeWall': 'string', 'CornerWallWidth': 'float', 'CorniceOverhang': 'float', 'FloorHeight': 'float', 'FrontWindowWidth': 'float',
                             'GroundfloorHeight': 'float', 'Mode': 'string', 'RearWindowWidth': 'float', 'SillSize': 'float', 'TileWidth': 'float', 'WallTexture': 'string', 'WindowHeight': 'float', 'streetWidth': 'float'}

        self.assertDictEqual(inspect_dict, ground_truth_dict)
