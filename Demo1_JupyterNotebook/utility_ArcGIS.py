# Python utility functions for ArcGIS API for Python/Notebook Demo
import sys, os
import numpy as np

SDK_PATH = os.path.join(os.getcwd(), "..", "install", "bin")
sys.path.append(SDK_PATH)

import pyprt
from arcgis.geometry import Geometry


def convert_ArcGIS_geometry(feature_set):
    initial_geometries = []
    try:
        for feature in feature_set.features:
            geo = Geometry(feature.geometry)
            if geo.type is 'Polygon':
                theArrayFromArcGIS = geo.coordinates()
                theAArray = theArrayFromArcGIS[0]
                a = theAArray[:-1]
                b = np.flip(a,axis=0)
                b[:,1] *= -1
                c = np.insert(b, 1, 0, axis=1)
                d = np.reshape(c,(1,c.shape[0]*c.shape[1]))
                initial_geometry = pyprt.Geometry(d.tolist()[0])
                initial_geometries.append(initial_geometry)
    except:
        print("This is not a feature set.")
    finally:
        return initial_geometries