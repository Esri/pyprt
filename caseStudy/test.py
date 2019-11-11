import sys, os

from PyPRT import pyprt, utility
import numpy as np
#help(p)


# Python bindings test
VAL = pyprt.print_val(407)
print("\nTest Function: it should print 407.")
print(VAL)


CS_FOLDER = os.path.dirname(os.path.realpath(__file__))

def asset_file(filename):
    return os.path.join(os.path.dirname(CS_FOLDER), "caseStudy", filename)

encoderOptions = {'outputPath': '/tmp/pyprt_output'}
os.makedirs(encoderOptions['outputPath'], exist_ok=True)

# PRT initialization
print("\nInitializing PRT.")
pyprt.initialize_prt()

if not pyprt.is_prt_initialized():
    raise Exception("PRT is not initialized")


# Python bindings PRT generation tests

## TEST 1: initial shape as OBJ, simple rule.
print("\nTEST1")
shape_geo_fromOBJ = asset_file("simple_scene_0.obj")
rpk = asset_file("simple_rule2019.rpk")
attrs = {'ruleFile' : "bin/simple_rule2019.cgb", 'startRule' : "Default$Footprint"}

mod_test1 = pyprt.ModelGenerator(shape_geo_fromOBJ)
models_test1 = mod_test1.generate_model(attrs, rpk, "com.esri.prt.examples.PyEncoder", {})

utility.visualize_PRT_results(models_test1)

## TEST 2: initial shape as DAE, simple rule.
print("\nTEST2")
shape_geo_fromDAE = asset_file("new_sceneCollada_0.dae")

mod_test2 = pyprt.ModelGenerator(shape_geo_fromDAE)
models_test2 = mod_test2.generate_model(attrs, rpk, "com.esri.prt.examples.PyEncoder", {})

utility.visualize_PRT_results(models_test2)

## TEST 3: initial shape as OBJ, simple rule, generated geometry outputted as OBJ.
print("\nTEST3")
OBJ_exporter = "com.esri.prt.codecs.OBJEncoder"

mod_test3 = pyprt.ModelGenerator(shape_geo_fromOBJ)
models_test3 = mod_test3.generate_model(attrs, rpk, OBJ_exporter, encoderOptions)
print("Results located in the output folder.")

## TEST 4: initial shapes as custom geometries, simple rule.
print("\nTEST4")
shape_geometry_1 = pyprt.Geometry([0, 0, 0,  0, 0, 1,  1, 0, 1,  1, 0, 0])
shape_geometry_2 = pyprt.Geometry([0, 0, 0,  0, 0, 1,  1, 0, 1,  1, 0, 0, 0.5, 0, 0.5])

mod_test4 = pyprt.ModelGenerator([shape_geometry_1,shape_geometry_2])
models_test4 = mod_test4.generate_model(attrs, rpk, "com.esri.prt.examples.PyEncoder", {})

utility.visualize_PRT_results(models_test4)

## TEST 5: initial shapes as OBJ, candler rule.
print("\nTEST5")
shape_geo_fromOBJ_test5 = asset_file("candler_footprint.obj")
mod_test5 = pyprt.ModelGenerator(shape_geo_fromOBJ_test5)

rpk_test5 = asset_file("candler.rpk")
attrs_test5 = {'ruleFile' : "bin/candler.cgb", 'startRule' : "Default$Footprint"}
models_test5 = mod_test5.generate_model(attrs_test5, rpk_test5, "com.esri.prt.examples.PyEncoder", {})

utility.visualize_PRT_results(models_test5)

## TEST 6: initial shapes as OBJ, candler rule, generated geometry outputted as Scene Layer Package.
print("\nTEST6")
mod_test6 = pyprt.ModelGenerator(shape_geo_fromOBJ_test5)
enc_optionsSLPK = {'layerTextureEncoding' : ["2"],'layerEnabled' : [True],'layerUID' : ["1"],'layerName' : ["TheLayer"],'layerTextureQuality' : [1.0],'layerTextureCompression' : [9],'layerTextureScaling': [1.0],'layerTextureMaxDimension' : [2048],'layerFeatureGranularity' : ["0"],'layerBackfaceCulling' : [False]}
enc_optionsSLPK.update(encoderOptions)

models_test6 = mod_test6.generate_model(attrs_test5, rpk_test5, "com.esri.prt.codecs.I3SEncoder", enc_optionsSLPK)

print("SLPK file located in the output folder.")

## TEST 7: initial shapes as OBJ, complex rule. (disabled by default due to large resources needed)
# print("\nTEST7")
# shape_geo_fromOBJ_test7 = asset_file("greenbuildingfootprint_0.obj")
# rpk_test7 = asset_file("envelope1806.rpk")
# attrs_test7 = {'ruleFile' : "rules/typology/envelope.cgb", 'startRule' : "Default$Lot", 'report_but_not_display_green' : "true"}

# mod_test7 = pyprt.ModelGenerator(shape_geo_fromOBJ_test7)
# models_test7 = mod_test7.generate_model(attrs_test7, rpk_test7, "com.esri.prt.examples.PyEncoder", {})

# utility.visualize_PRT_results(models_test7)


print("\nShutdown PRT.")
pyprt.shutdown_prt()