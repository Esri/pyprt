import sys
import os

from PyPRT import pyprt, utility

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_datafile(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'data', filename)

print('\nInitializing PRT.')
pyprt.initialize_prt()

if not pyprt.is_prt_initialized():
    raise Exception('PRT is not initialized')


### DATA
rpk1 = asset_datafile('candler.rpk')
attrs1 = {'ruleFile': 'bin/candler.cgb', 'startRule': 'Default$Footprint'}


### INITIAL SHAPES
shape_geometry_1 = pyprt.Geometry([0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
shape_geometry_2 = pyprt.Geometry([0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])


### PRT GENERATION
m1 = pyprt.ModelGenerator([shape_geometry_2, shape_geometry_1])

encoderOptions = {'outputPath': '/tmp/pyprt_output'}
os.makedirs(encoderOptions['outputPath'], exist_ok=True)

mo = m1.generate_model([attrs1], rpk1, 'com.esri.prt.codecs.OBJEncoder', encoderOptions)
utility.visualize_PRT_results(mo)


### PRT END
pyprt.shutdown_prt()
print('\nShutdown PRT.')