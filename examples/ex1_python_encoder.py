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

# Initial Shape
initial_shape1 = pyprt.InitialShape(
    [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])

# STEP 1: PRT Generation
print('\nFirst Generation: generated geometry + report\n')
m1 = pyprt.ModelGenerator([initial_shape1])
model1 = m1.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                           'emitGeometry': True, 'emitReport': True})
visualize_prt_results(model1)

# STEP 2: PRT Generation
print('\nSecond Generation: generated geometry\n')
m1 = pyprt.ModelGenerator([initial_shape1])
model1 = m1.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {
                           'emitGeometry': True, 'emitReport': False})
visualize_prt_results(model1)

# PRT End
print('\nShutdown PRT.')
pyprt.shutdown_prt()
