import os
import unittest

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

        encoder_options = {
            'outputPath': os.path.dirname(asset_output_file(''))}
        os.makedirs(encoder_options['outputPath'], exist_ok=True)

        shape_geo_from_obj = asset_file('greenbuildingfootprint_0.obj')
        rpk = asset_file('Building_From_FootprintSMALL.rpk')
        attrs = {'ruleFile': 'bin/Building_From_FootprintSMALL.cgb', 'startRule': 'Default$Generate',
                 'Reporting': 'All'}
        slpk_options = {'layerTextureEncoding': ['2'], 'layerEnabled': [True], 'layerUID': ['1'],
                        'layerName': ['Salut'], 'layerTextureQuality': [1.0], 'layerTextureCompression': [9],
                        'layerTextureScaling': [1.0], 'layerTextureMaxDimension': [2048],
                        'layerFeatureGranularity': ['0'], 'layerBackfaceCulling': [False], 'baseName': 'Unittest4SLPK'}
        slpk_options.update(encoder_options)
        m = pyprt.ModelGenerator(shape_geo_from_obj)
        m.generate_model(
            [attrs], rpk, 'com.esri.prt.codecs.I3SEncoder', slpk_options)
        self.assertTrue(os.path.isfile(
            asset_output_file('Unittest4SLPK.slpk')))
        self.assertGreater(
            os.stat(asset_output_file('CGAReport.txt')).st_size, 0)
