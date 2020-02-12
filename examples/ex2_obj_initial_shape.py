import sys
import os

import pyprt
from pyprt.pyprt_utils import visualize_prt_results

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))


def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'examples', 'data', filename)


# PRT Initialization
print('\nInitializing PRT.')
pyprt.initialize_prt()

if not pyprt.is_prt_initialized():
    raise Exception('PRT is not initialized')

# Data
rpk = asset_file('extrusion_rule.rpk')
attrs = {'ruleFile': 'bin/extrusion_rule.cgb',
         'startRule': 'Default$Footprint'}

# STEP 1: Initial Shape (vertices coordinates)
initial_shape1 = pyprt.InitialShape(
    [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])

# PRT Generation
print('\nFirst Generation:\n')
m1 = pyprt.ModelGenerator([initial_shape1])
model1 = m1.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
visualize_prt_results(model1)

# STEP 2: Initial Shape (OBJ file)
initial_shape2 = pyprt.InitialShape(asset_file('building_parcel.obj'))

# PRT Generation
print('\nSecond Generation:\n')
m2 = pyprt.ModelGenerator([initial_shape2])
model2 = m2.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {})
visualize_prt_results(model2)

# PRT End
print('\nShutdown PRT.')
pyprt.shutdown_prt()
