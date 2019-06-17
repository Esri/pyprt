import sys, os
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

if(not pyprt.is_prt_initialized()):
    raise Exception("PRT is not initialized")


mod2 = pyprt.ModelGenerator(asset_file("candler_footprint.obj"))
models2 = mod2.generate_model(asset_file("Building_From_Footprint5.rpk"), ["ruleFile:string=rules/Buildings/Building_From_Footprint.cgb", "startRule:string=Default$Generate", "Reporting:string=All"])

if(len(models2)>0):
    for model in models2: 
        geo2 = model.get_vertices()
        geo_numpy2 = np.array(geo2)
        print("\nSize of the matrix containing the model vertices:")
        print(geo_numpy2.shape)
        print(geo_numpy2)
        rep_float2 = model.get_float_report()
        rep_string2 = model.get_string_report()
        rep_bool2 = model.get_bool_report()
        print("Report of the generated model:")
        if(len(rep_float2)):
            print(rep_float2)
        if(len(rep_string2)):
            print(rep_string2)
        if(len(rep_bool2)):
            print(rep_bool2)
else:
    print("\nError while instanciating the model generator.")


print("\nShutdown PRT.")