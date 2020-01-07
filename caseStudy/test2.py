import sys
import os

import pyprt
from pyprt.pyprt_utils import visualize_prt_results

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), 'caseStudy', filename)

encoderOptions = {'outputPath': '/tmp/pyprt_output'}
os.makedirs(encoderOptions['outputPath'], exist_ok=True)

print('\nInitializing PRT.')
pyprt.initialize_prt()

if not pyprt.is_prt_initialized():
    raise Exception('PRT is not initialized')

# shapeGeo = asset_file('candler_footprint.obj')
# rpk = asset_file('Building_From_Footprint5.rpk')
# attrs = {'ruleFile': 'rules/Buildings/Building_From_Footprint.cgb', 'startRule': 'Default$Generate', 'Reporting': 'All'}
# attrs2 = {'Reporting': 'None'}

shapeGeo = asset_file('greenbuildingfootprint_0.obj')
rpk = asset_file('envelope1806.rpk')
attrs = {'ruleFile': 'rules/typology/envelope.cgb', 'startRule': 'Default$Lot', 'report_but_not_display_green': True}
attrs2 = {'ruleFile': 'rules/typology/envelope.cgb', 'startRule': 'Default$Lot', 'report_but_not_display_green': False}


m = pyprt.ModelGenerator(shapeGeo)
models = m.generate_model([attrs], rpk, 'com.esri.pyprt.PyEncoder', {'emitGeometry': False})

visualize_prt_results(models)

models2 = m.generate_model([attrs2])

visualize_prt_results(models2)

models3 = m.generate_model([attrs], rpk, 'com.esri.prt.codecs.OBJEncoder', encoderOptions)

pyprt.shutdown_prt()
print('\nShutdown PRT.')