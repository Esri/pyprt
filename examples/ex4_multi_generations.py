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
attrs1 = {'ruleFile': 'bin/extrusion_rule.cgb',
          'startRule': 'Default$Footprint'}
attrs2 = {'ruleFile': 'bin/extrusion_rule.cgb',
          'startRule': 'Default$Footprint', 'minBuildingHeight': 30.0}
attrs3 = {'ruleFile': 'bin/extrusion_rule.cgb',
          'startRule': 'Default$Footprint', 'text': 'hello'}

# Initial Shapes
shape_geometry_1 = pyprt.InitialShape(
    [0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
shape_geometry_2 = pyprt.InitialShape(
    [0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])


# PRT Generation
m = pyprt.ModelGenerator([shape_geometry_1, shape_geometry_2])

print('\nFirst generation:\n')
models1 = m.generate_model([attrs1, attrs2], rpk,
                           'com.esri.pyprt.PyEncoder', {})
visualize_prt_results(models1)

print('\Second generation:\n')
# if only one shapes attributes dictionary is given, it applies to all initial shapes.
# in this case, equivalent to m1.generate_model([attrs3, attrs3]).
models2 = m.generate_model([attrs3])
visualize_prt_results(models2)


# PRT End
pyprt.shutdown_prt()
print('\nShutdown PRT.')
