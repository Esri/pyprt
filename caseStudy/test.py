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

mod = pyprt.ModelGenerator("C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/caseStudy/simple_scene_0.obj", "C:/Users/cami9495/Documents/esri-cityengine-sdk-master/examples/py4prt/caseStudy/simple_rule2019.rpk", ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModelSuper"])

if(mod.generate_model()):
    geo = mod.get_model_geometry()
    geo_numpy = np.array(geo)
    print("\nSize of the matrix containing all the model vertices:")
    print(geo_numpy.shape)
    rep = mod.get_model_report()
    print("\nReport of the generated model:")
    print(rep)
else:
    print("Error while instanciating ModelGenerator.")
