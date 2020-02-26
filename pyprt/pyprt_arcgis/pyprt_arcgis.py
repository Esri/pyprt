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
