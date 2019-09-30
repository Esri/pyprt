import sys, os
sys.path.append(os.path.join(os.getcwd(), "src"))
from utility import visualize_PRT_results

SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt
import numpy as np
#help(p)

VAL = pyprt.print_val(407)
print("\nTest Function: it should print 407.")
print(VAL)

CS_FOLDER = os.getcwd()
def asset_file(filename):
    return os.path.join(CS_FOLDER, "caseStudy", filename)

print("\nInitializing PRT.")
pyprt.initialize_prt(SDK_PATH)

if not pyprt.is_prt_initialized():
    raise Exception("PRT is not initialized")

# shapeGeo = asset_file("candler_footprint.obj")
# rpk = asset_file("Building_From_Footprint5.rpk")
# attrs = {'ruleFile' : "rules/Buildings/Building_From_Footprint.cgb", 'startRule' : "Default$Generate", 'Reporting' : "All"}
# attrs2 = {'Reporting' : "None"}

shapeGeo = asset_file("greenbuildingfootprint_0.obj")
rpk = asset_file("envelope1806.rpk")
attrs = {'ruleFile' : "rules/typology/envelope.cgb", 'startRule' : "Default$Lot", 'report_but_not_display_green' : True}
attrs2 = {'report_but_not_display_green' : False}


m = pyprt.ModelGenerator(shapeGeo)
models = m.generate_model(attrs, rpk, "com.esri.prt.examples.PyEncoder", {'emitGeometry' : False})

visualize_PRT_results(models)

models2 = m.generate_model(attrs2)

visualize_PRT_results(models2)

models3 = m.generate_model(attrs, rpk, "com.esri.prt.codecs.OBJEncoder", {})

pyprt.shutdown_prt()
print("\nShutdown PRT.")