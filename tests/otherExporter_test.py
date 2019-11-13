import unittest
import os
import sys

SDK_PATH = os.path.join(os.getcwd(), 'build', 'lib.win-amd64-3.6', 'PyPRT', 'pyprt', 'bin')
sys.path.append(SDK_PATH)

import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'data', filename)

def asset_output_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'output', filename)


class ExporterTest(unittest.TestCase):
    def test_slpk(self):
        if os.path.isfile(asset_output_file('Unittest4SLPK.slpk')):
            os.remove(asset_output_file('Unittest4SLPK.slpk'))

        encoderOptions = {'outputPath': os.path.dirname(asset_output_file(''))}
        os.makedirs(encoderOptions['outputPath'], exist_ok=True)

        shape_geo_from_obj = asset_file('greenbuildingfootprint_0.obj')
        rpk = asset_file('Building_From_FootprintSMALL.rpk')
        attrs = {'ruleFile' : 'bin/Building_From_FootprintSMALL.cgb', 'startRule' : 'Default$Generate', 'Reporting' : 'All'}
        SLPKoptions = {'layerTextureEncoding' : ['2'],'layerEnabled' : [True],'layerUID' : ['1'],'layerName' : ['Salut'],'layerTextureQuality' : [1.0],'layerTextureCompression' : [9],'layerTextureScaling': [1.0],'layerTextureMaxDimension' : [2048],'layerFeatureGranularity' : ['0'],'layerBackfaceCulling' : [False], 'baseName' : 'Unittest4SLPK'}        
        SLPKoptions.update(encoderOptions)
        m = pyprt.ModelGenerator(shape_geo_from_obj)
        model = m.generate_model(attrs, rpk, 'com.esri.prt.codecs.I3SEncoder', SLPKoptions)
        self.assertTrue(os.path.isfile(asset_output_file('Unittest4SLPK.slpk')))
        self.assertGreater(os.stat(asset_output_file('CGAReport.txt')).st_size, 0)
