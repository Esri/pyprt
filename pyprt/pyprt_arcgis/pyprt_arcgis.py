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
    """arcgis_to_pyprt(feature_set) -> List[InitialShape]
    This function allows converting an ArcGIS FeatureSet into a list of PyPRT InitialShape instances.
    You then typically call the ModelGenerator constructor with the return value if this function as parameter.

    Parameters:
        feature_set: FeatureSet

    Returns:
        List[InitialShape]

    """
    initial_geometries = []
    for feature in feature_set.features:
        try:
            geo = Geometry(feature.geometry)
            if geo.type == 'Polygon' and (not geo.is_empty):
                pts_cnt = 0
                vert_coord_list = []
                face_count_list = []
                holes_ind_list = []
                coord_list = geo.coordinates()

                for face_idx, coord_part in enumerate(coord_list):
                    in_geo = Geometry({"rings": [coord_part]})
                    store_area = in_geo.area
                    coord_remove_last = coord_part[:-1]
                    coord_inverse = np.flip(coord_remove_last, axis=0)

                    if len(coord_part[0]) == 2:  # we have to add a dimension
                        coord_inverse[:, 1] *= -1
                        coord_add_dim = np.insert(coord_inverse, 1, 0, axis=1)
                        coord_fin = np.reshape(
                            coord_add_dim, (1, coord_add_dim.shape[0]*coord_add_dim.shape[1]))
                    elif len(coord_part[0]) == 3:  # need to swap the 1 and 2 columns
                        coord_inverse[:, 1] *= -1
                        coord_swap_dim = coord_inverse.copy()
                        temp = np.copy(coord_swap_dim[:, 1])
                        coord_swap_dim[:, 1] = coord_swap_dim[:, 2]
                        coord_swap_dim[:, 2] = temp
                        coord_fin = np.reshape(
                            coord_swap_dim, (1, coord_swap_dim.shape[0]*coord_swap_dim.shape[1]))
                    else:
                        print("Only 2D or 3D points are supported.")

                    vert_coord_list.extend(coord_fin[0])
                    nb_pts = len(coord_fin[0])/3
                    pts_cnt += nb_pts
                    face_count_list.append(int(nb_pts))
                    if store_area > 0.0: # interior ring / holes
                        holes_ind_list.append(face_idx)

                # holes face index to PRT holes list of lists conversion
                face_indices_list = list(range(0, sum(face_count_list)))
                holes_dict = {}
                holes_list = []
                if len(holes_ind_list) > 0:
                    for h_idx in holes_ind_list:
                        f_idx = h_idx
                        while f_idx > 0:
                            f_idx -= 1
                            if not (f_idx in holes_ind_list):
                                if not (f_idx in holes_dict):
                                    holes_dict[f_idx] = [h_idx]
                                else:
                                    holes_dict[f_idx].append(h_idx)
                                break

                    for key, value in holes_dict.items():
                        face_holes = [key]
                        face_holes.extend(value)
                        holes_list.append(face_holes)
                
                initial_geometry = pyprt.InitialShape(vert_coord_list, face_indices_list, face_count_list, holes_list)
                initial_geometries.append(initial_geometry)
        except:
            print("This feature is not valid: ")
            print(feature)
            print()
    return initial_geometries
