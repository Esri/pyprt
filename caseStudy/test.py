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

mod = pyprt.ModelGenerator(asset_file("simple_scene_0.obj"), asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModelSuper"])

if(mod.generate_model()):
    geo = mod.get_model_geometry()
    geo_numpy = np.array(geo)
    print("\nSize of the matrix containing all the model vertices:")
    print(geo_numpy.shape)
    rep_float = mod.get_model_float_report()
    rep_string = mod.get_model_string_report()
    rep_bool = mod.get_model_bool_report()
    print("\nReport of the generated model:")
    if(len(rep_float)):
        print(rep_float)
    if(len(rep_string)):
        print(rep_string)
    if(len(rep_bool)):
        print(rep_bool)
else:
    print("\nError while instanciating the model generator.")


mod2 = pyprt.ModelGenerator(asset_file("candler_footprint.obj"), asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModelSuper"])

if(mod2.generate_model()):
    geo2 = mod2.get_model_geometry()
    geo_numpy2 = np.array(geo2)
    print("\nSize of the matrix containing all the model vertices:")
    print(geo_numpy2.shape)
    rep_float2 = mod2.get_model_float_report()
    rep_string2 = mod2.get_model_string_report()
    rep_bool2 = mod2.get_model_bool_report()
    print("\nReport of the generated model:")
    if(len(rep_float2)):
        print(rep_float2)
    if(len(rep_string2)):
        print(rep_string2)
    if(len(rep_bool2)):
        print(rep_bool2)
else:
    print("\nError while instanciating the model generator.")

mod3 = pyprt.ModelGenerator(asset_file("new_sceneCollada_0.dae"), asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModelSuper"])

if(mod3.generate_model()):
    geo3 = mod3.get_model_geometry()
    geo_numpy3 = np.array(geo3)
    print("\nSize of the matrix containing all the model vertices:")
    print(geo_numpy3.shape)
    rep_float3 = mod3.get_model_float_report()
    rep_string3 = mod3.get_model_string_report()
    rep_bool3 = mod3.get_model_bool_report()
    print("\nReport of the generated model:")
    if(len(rep_float3)):
        print(rep_float3)
    if(len(rep_string3)):
        print(rep_string3)
    if(len(rep_bool3)):
        print(rep_bool3)
else:
    print("\nError while instanciating the model generator.")

print("\nShutdown PRT.")
pyprt.shutdown_prt()