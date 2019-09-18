import unittest
import os
import sys

CS_FOLDER = os.getcwd()
SDK_PATH = os.path.join(CS_FOLDER, "install", "bin")
sys.path.append(SDK_PATH)

import pyprt

def asset_file(filename):
    return os.path.join(CS_FOLDER, "data", filename)

def asset_output_file(filename):
    return os.path.join(CS_FOLDER, "output", filename)


class ExporterTest(unittest.TestCase):
    def test_slpk(self):
        if os.path.isfile(asset_output_file("Unittest4SLPK.slpk")):
            os.remove(asset_output_file("Unittest4SLPK.slpk"))

        shape_geo_from_obj = asset_file("greenbuildingfootprint_0.obj")
        rpk = asset_file("Building_From_FootprintSMALL.rpk")
        attrs = {'ruleFile' : "bin/Building_From_FootprintSMALL.cgb", 'startRule' : "Default$Generate", 'Reporting' : "All"}
        SLPKoptions = {'layerTextureEncoding' : ["2"],'layerEnabled' : [True],'layerUID' : ["1"],'layerName' : ["Salut"],'layerTextureQuality' : [1.0],'layerTextureCompression' : [9],'layerTextureScaling': [1.0],'layerTextureMaxDimension' : [2048],'layerFeatureGranularity' : ["0"],'layerBackfaceCulling' : [False], 'baseName' : "Unittest4SLPK"}        
        m = pyprt.ModelGenerator(shape_geo_from_obj)
        model = m.generate_model(rpk, attrs, SLPKoptions, "com.esri.prt.codecs.I3SEncoder")
        self.assertTrue(os.path.isfile(asset_output_file("Unittest4SLPK.slpk")))
        self.assertGreater(os.stat(asset_output_file("CGAReport.txt")).st_size, 0)


if __name__ == '__main__':
    unittest.main()