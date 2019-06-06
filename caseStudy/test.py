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

mod = pyprt.ModelGenerator(asset_file("simple_scene_0.obj"))
models = mod.generate_model(asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"])

if(len(models)>0):
    for model in models:
        geo = model.get_vertices()
        geo_numpy = np.array(geo)
        print("\nSize of the matrix containing the model vertices:")
        print(geo_numpy.shape)
        rep_float = model.get_float_report()
        rep_string = model.get_string_report()
        rep_bool = model.get_bool_report()
        print("Report of the generated model:")
        if(len(rep_float)):
            print(rep_float)
        if(len(rep_string)):
            print(rep_string)
        if(len(rep_bool)):
            print(rep_bool)
else:
    print("\nError while instanciating the model generator.")


mod2 = pyprt.ModelGenerator(asset_file("candler_footprint.obj"))
models2 = mod2.generate_model(asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"])

if(len(models2)>0):
    for model in models2:
        geo2 = model.get_vertices()
        geo_numpy2 = np.array(geo2)
        print("\nSize of the matrix containing the model vertices:")
        print(geo_numpy2.shape)
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

mod3 = pyprt.ModelGenerator(asset_file("new_sceneCollada_0.dae"))
models3 = mod3.generate_model(asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"])

if(len(models3)>0):
    for model in models3:
        geo3 = model.get_vertices()
        geo_numpy3 = np.array(geo3)
        print("\nSize of the matrix containing the model vertices:")
        print(geo_numpy3.shape)
        rep_float3 = model.get_float_report()
        rep_string3 = model.get_string_report()
        rep_bool3 = model.get_bool_report()
        print("Report of the generated model:")
        if(len(rep_float3)):
            print(rep_float3)
        if(len(rep_string3)):
            print(rep_string3)
        if(len(rep_bool3)):
            print(rep_bool3)
else:
    print("\nError while instanciating the model generator.")

v4 = [0, 0, 0,  0, 0, 1,  1, 0, 1,  1, 0, 0]
v41 = [0, 0, 0,  0, 0, 1,  1, 0, 1,  1, 0, 0, 0.5, 0, 0.5]
initialGeometry4 = pyprt.Geometry(v4)
initialGeometry41 = pyprt.Geometry(v41)

mod4 = pyprt.ModelGenerator([initialGeometry4,initialGeometry41])
all_models = mod4.generate_model(asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"])

print("\nNumber of generated models: "+ str(len(all_models)))
if(len(all_models)>0):
    for model in all_models:
        geo4 = model.get_vertices()
        geo_numpy4 = np.array(geo4)
        print("\nSize of the matrix containing the model vertices:")
        print(geo_numpy4.shape)
        rep_float4 = model.get_float_report()
        rep_string4 = model.get_string_report()
        rep_bool4 = model.get_bool_report()
        print("Report of the generated model:")
        if(len(rep_float4)):
            print(rep_float4)
        if(len(rep_string4)):
            print(rep_string4)
        if(len(rep_bool4)):
            print(rep_bool4)
else:
     print("\nError while instanciating the model generator.")


models4bis = mod4.generate_model(asset_file("candler.rpk"), ["ruleFile:string=bin/candler.cgb", "startRule:string=Default$Footprint"])

print("\nNumber of generated models: "+ str(len(models4bis)))
if(len(models4bis)>0):
    for model in models4bis:
        geo4bis = model.get_vertices()
        geo_numpy4bis = np.array(geo4bis)
        print("\nSize of the matrix containing the model vertices:")
        print(geo_numpy4bis.shape)
        rep_float4bis = model.get_float_report()
        rep_string4bis = model.get_string_report()
        rep_bool4bis = model.get_bool_report()
        print("Report of the generated model:")
        if(len(rep_float4bis)):
            print(rep_float4bis)
        if(len(rep_string4bis)):
            print(rep_string4bis)
        if(len(rep_bool4bis)):
            print(rep_bool4bis)
else:
     print("\nError while instanciating the model generator.")


model2bis = mod2.generate_model(asset_file("candler.rpk"), ["ruleFile:string=bin/candler.cgb", "startRule:string=Default$Footprint"],"com.esri.prt.codecs.OBJEncoder")


print("\nShutdown PRT.")
pyprt.shutdown_prt()