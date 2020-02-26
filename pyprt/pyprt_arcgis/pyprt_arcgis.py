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
import sys

import numpy as np
import pyprt

try:
    from arcgis.geometry import Geometry
except ModuleNotFoundError:
    sys.exit("This module can be imported only if arcgis package is installed.")


def arcgis_to_pyprt(feature_set):
    initial_geometries = []
    for feature in feature_set.features:
        try:
            geo = Geometry(feature.geometry)
            if geo.type is 'Polygon':
                coord = geo.coordinates()[0]
                coord_remove_last = coord[:-1]
                coord_inverse = np.flip(coord_remove_last, axis=0)

                if coord.shape[1] == 2:  # we have to add a dimension
                    coord_inverse[:, 1] *= -1
                    coord_add_dim = np.insert(coord_inverse, 1, 0, axis=1)
                    coord_fin = np.reshape(
                        coord_add_dim, (1, coord_add_dim.shape[0]*coord_add_dim.shape[1]))
                elif coord.shape[1] == 3:  # need to swap the 1 and 2 columns
                    coord_inverse[:, 1] *= -1
                    coord_swap_dim = coord_inverse.copy()
                    temp = np.copy(coord_swap_dim[:, 1])
                    coord_swap_dim[:, 1] = coord_swap_dim[:, 2]
                    coord_swap_dim[:, 2] = temp
                    coord_fin = np.reshape(
                        coord_swap_dim, (1, coord_swap_dim.shape[0]*coord_swap_dim.shape[1]))

                initial_geometry = pyprt.InitialShape(coord_fin.tolist()[0])
                initial_geometries.append(initial_geometry)
        except:
            print("This feature is not valid: ")
            print(feature)
            print()
    return initial_geometries
