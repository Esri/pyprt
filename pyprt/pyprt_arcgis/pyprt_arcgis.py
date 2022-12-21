# Copyright (c) 2012-2022 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   https://www.apache.org/licenses/LICENSE-2.0

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


def add_dimension(array_coord_2d):
    array_coord_3d = np.insert(array_coord_2d, 1, 0, axis=1)
    return np.reshape(array_coord_3d, (1, array_coord_3d.shape[0] * array_coord_3d.shape[1]))


def swap_yz_dimensions(array_coord):
    coord_swap_dim = array_coord.copy()
    temp = np.copy(array_coord[:, 1])
    coord_swap_dim[:, 1] = coord_swap_dim[:, 2]
    coord_swap_dim[:, 2] = temp
    return np.reshape(coord_swap_dim, (1, coord_swap_dim.shape[0] * coord_swap_dim.shape[1]))


def get_hole_info(geo):
    # use Shapely to detect interior rings (holes)
    # doing the ccw check is a workaround as sometimes the exterior is actually a hole
    # we assume that interior rings follow "their" exterior ring

    shapely_geo = geo.as_shapely

    shapely_rings = []
    for shapely_part in shapely_geo.geoms:
        shapely_rings.append(shapely_part.exterior)
        for shapely_interior in shapely_part.interiors:
            shapely_rings.append(shapely_interior)

    holes = []
    hole = []
    shapely_face_index = 0
    for shapely_ring in shapely_rings:
        if not shapely_ring.is_ccw:  # exterior
            if len(hole) > 0:
                holes.append(hole)
            hole = [shapely_face_index]
        else:  # interior
            hole.append(shapely_face_index)
        shapely_face_index += 1
    if len(hole) > 0:
        holes.append(hole)

    return holes


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
                coord_list = geo.coordinates()

                for face_idx, coord_part in enumerate(coord_list):
                    coord_remove_last = coord_part[:-1]
                    coord_inverse = np.flip(coord_remove_last, axis=0)
                    coord_inverse[:, 1] *= -1

                    if len(coord_part[0]) == 2:
                        coord_fin = add_dimension(coord_inverse)
                    elif len(coord_part[0]) == 3:
                        coord_fin = swap_yz_dimensions(coord_inverse)
                    else:
                        raise "Only 2D or 3D points are supported."

                    vert_coord_list.extend(coord_fin[0])
                    nb_pts = len(coord_fin[0]) / 3
                    pts_cnt += nb_pts
                    face_count_list.append(int(nb_pts))

                face_indices_list = list(range(0, sum(face_count_list)))
                holes = get_hole_info(geo)

                initial_geometry = pyprt.InitialShape(vert_coord_list, face_indices_list, face_count_list, holes)
                initial_geometries.append(initial_geometry)
        except:
            print(f'Ignoring invalid feature with id = {feature.get_value("objectid")}.')
    return initial_geometries
