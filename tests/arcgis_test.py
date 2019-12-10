import os
import unittest

import pyprt


class ArcGISAPITest(unittest.TestCase):
    def test_import(self):

        from arcgis.gis import GIS
        from arcgis.geometry import Geometry
        from pyprt.pyprt_arcgis import arcgis_to_pyprt

        gis = GIS()
        item = gis.content.get('b1598d3df2c047ef88251016af5b0f1e')
        id_list = (105, 129)
        fset = item.layers[0].query(
            where='OBJECTID IN'+str(id_list), return_z=True)

        initial_geometries = arcgis_to_pyprt(fset)

        self.assertEqual(len(initial_geometries), 2)
