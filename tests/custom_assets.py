import os
import unittest
import pyprt

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
	return os.path.join(os.path.dirname(CS_FOLDER), 'tests', 'data', filename)


class CustomAssetTests(unittest.TestCase):
	def test_something(self):
		rpk = asset_file('extrusion_rule.rpk')
		shape_geometry_1 = pyprt.InitialShape([0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])
		m = pyprt.ModelGenerator([shape_geometry_1])
		assets = {"CandlerBuilding.glb": "https://github.com/esri/pyprt-examples/blob/main/data/CandlerBuilding.glb"}
		model = m.generate_model([dict()], rpk, 'com.esri.pyprt.PyEncoder', dict(), assets)


if __name__ == '__main__':
	unittest.main()
