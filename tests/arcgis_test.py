import os
import unittest

import pyprt


class ArcGISAPITest(unittest.TestCase):
    def test_import(self):

        from arcgis.gis import GIS
        from pyprt.pyprt_arcgis import arcgis_to_pyprt

        gis = GIS()
        item = gis.content.get('6ddd4741514d4e47b005c4962f06de58')
        fset = item.layers[0].query(return_z=True)

        initial_geometries = arcgis_to_pyprt(fset)

        self.assertEqual(len(initial_geometries), 2)
