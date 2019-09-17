import unittest
import os
import sys

CS_FOLDER = os.getcwd()
SDK_PATH = os.path.join(CS_FOLDER, "install", "bin")
sys.path.append(SDK_PATH)

import pyprt

def asset_file(filename):
    return os.path.join(CS_FOLDER, "data", filename)


class MultiTest(unittest.TestCase):
    def test_multigenerations(self):
        rpk = asset_file("simple_rule0819.rpk")
        attrs = {'ruleFile' : "bin/simple_rule2019.cgb", 'startRule' : "Default$Footprint"}
        shape_geo = pyprt.Geometry([-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 10.0, 0.0, 10.0])
        m = pyprt.ModelGenerator([shape_geo])
        model1 = m.generate_model(rpk, attrs)
        model2 = m.generate_another_model(attrs, {'emitReports' : False})
        model3 = m.generate_another_model(attrs, {'emitReports' : True, 'emitGeometry' : False})
        model4 = m.generate_another_model(attrs, {'emitGeometry' : True})
        self.assertDictEqual(model1[0].get_float_report(), model4[0].get_float_report())
        self.assertDictEqual(model1[0].get_float_report(), model3[0].get_float_report())
        self.assertListEqual(model1[0].get_vertices(), model4[0].get_vertices())
        self.assertListEqual(model2[0].get_vertices(), model4[0].get_vertices())


if __name__ == '__main__':
    unittest.main()