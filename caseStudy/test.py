import sys

try:
    sys.path.index("C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/install/bin")
except ValueError:
    sys.path.append("C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/install/bin")


import pyprt
import numpy as np
#help(p)

VAL = pyprt.print_val(407)
print("\nTest Function: it should print 407.")
print(VAL)

print("\nInitializing PRT:")
pyprt.ModelGenerator.initialize_prt()

mod = pyprt.ModelGenerator("C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/caseStudy/simple_scene_0.obj", "C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/caseStudy/simple_rule2019.rpk", ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModelSuper"])

if(mod.is_prt_initialized()):
    if(mod.generate_model()):
        geo = mod.get_model_geometry()
        geo_numpy = np.array(geo)
        print("\nSize of the matrix containing all the model vertices:")
        print(geo_numpy.shape)
        rep = mod.get_model_report()
        print("\nReport of the generated model:")
        print(rep)
    else:
        print("\nError while instanciating the model generator.")
else:
    print("\nPRT is not initialized.")


mod2 = pyprt.ModelGenerator("C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/caseStudy/candler_footprint.obj", "C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/caseStudy/simple_rule2019.rpk", ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModelSuper"])

if(mod2.is_prt_initialized()):
    if(mod2.generate_model()):
        geo2 = mod2.get_model_geometry()
        geo_numpy2 = np.array(geo2)
        print("\nSize of the matrix containing all the model vertices:")
        print(geo_numpy2.shape)
        rep2 = mod2.get_model_report()
        print("\nReport of the generated model:")
        print(rep2)
    else:
        print("\nError while instanciating the model generator.")
else:
    print("\nPRT is not initialized.")
