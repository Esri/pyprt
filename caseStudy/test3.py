import sys, os

from PyPRT import pyprt, utility
import numpy as np


VAL = pyprt.print_val(407)
print('\nTest Function: it should print 407.')
print(VAL)

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_datafile(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'data', filename)

print('\nInitializing PRT.')
pyprt.initialize_prt()

if not pyprt.is_prt_initialized():
    raise Exception('PRT is not initialized')


### DATA
rpk1 = asset_datafile('test_rule.rpk')
attrs1 = {'ruleFile' : 'bin/test_rule.cgb', 'startRule' : 'Default$Footprint'}
attrs2 = {'ruleFile' : 'bin/test_rule.cgb', 'startRule' : 'Default$Footprint', 'minBuildingHeight' : 30.0}
attrs3 = {'ruleFile' : 'bin/test_rule.cgb', 'startRule' : 'Default$Footprint', 'text' : 'hello'}

### INITIAL SHAPES
shape_geometry_1 = pyprt.Geometry([0, 0, 0,  0, 0, 100,  100, 0, 100,  100, 0, 0])
shape_geometry_2 = pyprt.Geometry([0, 0, 0,  0, 0, -10,  -10, 0, -10,  -10, 0, 0, -5, 0, -5])


### PRT GENERATION
m1 = pyprt.ModelGenerator([shape_geometry_2, shape_geometry_1])

mo = m1.generate_model([attrs1, attrs2], rpk1, 'com.esri.pyprt.PyEncoder', {})#, {'emitReport' : False, 'emitGeometry' : False})
utility.visualize_PRT_results(mo)

mo2 = m1.generate_model([attrs3])
utility.visualize_PRT_results(mo2)


### PRT END
pyprt.shutdown_prt()
print('\nShutdown PRT.')