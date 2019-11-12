import sys, os

from PyPRT import pyprt, utility
import numpy as np
#help(p)

VAL = pyprt.print_val(407)
print("\nTest Function: it should print 407.")
print(VAL)

CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), "caseStudy", filename)

encoderOptions = {'outputPath': '/tmp/pyprt_output'}
os.makedirs(encoderOptions['outputPath'], exist_ok=True)

print("\nInitializing PRT.")
pyprt.initialize_prt()

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

utility.visualize_PRT_results(models)

models2 = m.generate_model(attrs2)

utility.visualize_PRT_results(models2)

models3 = m.generate_model(attrs, rpk, "com.esri.prt.codecs.OBJEncoder", encoderOptions)

pyprt.shutdown_prt()
print("\nShutdown PRT.")