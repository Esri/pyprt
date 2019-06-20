import sys, os
SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt
import numpy as np
#help(p)


# Python bindings test
VAL = pyprt.print_val(407)
print("\nTest Function: it should print 407.")
print(VAL)


# Python utils functions
CS_FOLDER = os.getcwd()
def asset_file(filename):
    return os.path.join(CS_FOLDER, "caseStudy", filename)

def summarize_report(report):
    summary_dict = {}
    for x in report.values():
        if(len(x)>0):
            for y in x:
                if y in summary_dict:
                    summary_dict[y] += x[y]
                else:
                    summary_dict[y] = x[y]    
    return summary_dict

def summarize_matrix(vertices_matrix):
    gathered_matrix = []
    for x in vertices_matrix.values():
        gathered_matrix.extend(x)
    return gathered_matrix

def visualize_PRT_results(models):
    if len(models) > 0:
        print("\nNumber of generated geometries (= nber of initial shapes):")
        print(len(models))

        for model in models:

            geometry_vertices = model.get_vertices()
            geo_summarized = summarize_matrix(geometry_vertices)
            geo_numpy = np.array(geo_summarized)
            print("Size of the matrix containing the model vertices:")
            print(geo_numpy.shape)

            rep_float = model.get_float_report()
            rep_string = model.get_string_report()
            rep_bool = model.get_bool_report()
            print("Reports of the generated model (floats report, strings report, bools report):")
            print(rep_float)
            print(rep_string)
            print(rep_bool)
            print("Number of subshapes:")
            print(len(rep_float))
            print("Summarized Reports over all subshapes:")
            if len(summarize_report(rep_float)) > 0:
                print(summarize_report(rep_float))
            if len(summarize_report(rep_string)) > 0:
                print(summarize_report(rep_string))
            if len(summarize_report(rep_bool)) > 0:
                print(summarize_report(rep_bool))
    else:
        print("\nError while instanciating the model generator.")


# PRT initialization
print("\nInitializing PRT.")
pyprt.initialize_prt(SDK_PATH)

if(not pyprt.is_prt_initialized()):
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

## TEST 6: initial shapes as custom geometry, complex rule.
print("\nTEST6")
shape_geo_fromOBJ_test6 = asset_file("greenbuildingfootprint_0.obj")
rpk_test6 = asset_file("envelope1806.rpk")
attrs_test6 = ["ruleFile:string=rules/typology/envelope.cgb", "startRule:string=Default$Lot", "report_but_not_display_green:string=true"]

mod_test6 = pyprt.ModelGenerator(shape_geo_fromOBJ_test6)
models_test6 = mod_test6.generate_model(rpk_test6, attrs_test6)

visualize_PRT_results(models_test6)


print("\nShutdown PRT.")
pyprt.shutdown_prt()