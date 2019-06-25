import sys, os
sys.path.append(os.path.join(os.getcwd(), "src"))
from utility import visualize_PRT_results, combine_reports, summarize_matrix, summarize_report

SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt
import numpy as np
#help(p)


# Python bindings test
VAL = pyprt.print_val(407)
print("\nTest Function: it should print 407.")
print(VAL)


CS_FOLDER = os.getcwd()
def asset_file(filename):
    return os.path.join(CS_FOLDER, "caseStudy", filename)


# PRT initialization
print("\nInitializing PRT.")
pyprt.initialize_prt(SDK_PATH)

if not pyprt.is_prt_initialized():
    raise Exception("PRT is not initialized")


# Python bindings PRT generation tests

## TEST 1: initial shape as OBJ, simple rule.
print("\nTEST1")
shape_geo_fromOBJ = asset_file("simple_scene_0.obj")
rpk = asset_file("simple_rule2019.rpk")
attrs = ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"]


mod_test1 = pyprt.ModelGenerator(shape_geo_fromOBJ)
models_test1 = mod_test1.generate_model(rpk, attrs)

visualize_PRT_results(models_test1)

## TEST 2: initial shape as DAE, simple rule.
print("\nTEST2")
shape_geo_fromDAE = asset_file("new_sceneCollada_0.dae")


mod_test2 = pyprt.ModelGenerator(shape_geo_fromDAE)
models_test2 = mod_test2.generate_model(rpk, attrs)

visualize_PRT_results(models_test2)

## TEST 3: initial shape as DAE, simple rule.
print("\nTEST3")
OBJ_exporter = "com.esri.prt.codecs.OBJEncoder"

mod_test3 = pyprt.ModelGenerator(shape_geo_fromOBJ)
models_test3 = mod_test3.generate_model(rpk, attrs, OBJ_exporter)
print("Results located in the output folder.")

## TEST 4: initial shapes as custom geometries, simple rule.
print("\nTEST4")
shape_geometry_1 = pyprt.Geometry([0, 0, 0,  0, 0, 1,  1, 0, 1,  1, 0, 0])
shape_geometry_2 = pyprt.Geometry([0, 0, 0,  0, 0, 1,  1, 0, 1,  1, 0, 0, 0.5, 0, 0.5])

mod_test4 = pyprt.ModelGenerator([shape_geometry_1,shape_geometry_2])
models_test4 = mod_test4.generate_model(rpk, attrs)

visualize_PRT_results(models_test4)

## TEST 5: initial shapes as custom geometries, candler rule.
print("\nTEST5")
rpk_test5 = asset_file("candler.rpk")
attrs_test5 = ["ruleFile:string=bin/candler.cgb", "startRule:string=Default$Footprint"]
models_test5 = mod_test4.generate_model(rpk_test5, attrs_test5)

visualize_PRT_results(models_test5)


print("\nShutdown PRT.")
pyprt.shutdown_prt()