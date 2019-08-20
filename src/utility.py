# Python utility functions
import numpy as np
import copy

def collect_initial_shape_indices(report_or_matrix):
    generation_indices = []
    for x in report_or_matrix:
        if generation_indices.count(x) == 0:
            generation_indices.append(x)
    return generation_indices


def visualize_PRT_results(model, printing = False): ###
    if model:
        geometry_vertices = model.get_vertices()
        rep_float = model.get_float_report()
        rep_string = model.get_string_report()
        rep_bool = model.get_bool_report()
        combined_rep = combine_reports(model)

        if len(geometry_vertices) > 0:
            initial_shape_idx = collect_initial_shape_indices(geometry_vertices)
        elif len(combined_rep) > 0:
            initial_shape_idx = collect_initial_shape_indices(combined_rep)
        else:
            return

        print("\nNumber of generated geometries (= nber of initial shapes):")
        print(len(initial_shape_idx))

        for i in initial_shape_idx:
            print()
            print("Initial Shape Index: " + str(i))

            if len(geometry_vertices) > 0:
                print()
                geo_numpy = np.array(geometry_vertices[i])
                geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
                print("Size of the matrix containing the model vertices (no duplicates): " + str(geo_numpy[indices].shape))

                print("Size of the matrix containing the model faces: " + str(np.array(model.get_faces()[i]).shape))

            if len(combined_rep) > 0 :
                print()
                print("Report of the generated model:")
                print(combined_rep[i])

                print()
                if printing :
                    print("Reports of the generated model (floats report, strings report, bools report):")
                    print(rep_float)
                    print(rep_string)
                    print(rep_bool)
    else:
        print("\nError while instanciating the model generator.")


def combine_reports(model):
    combined_dict = {}
    for x in model.get_float_report():
        dict_per_ind = {}
        dict_per_ind.update(model.get_float_report()[x])
        dict_per_ind.update(model.get_string_report()[x])
        dict_per_ind.update(model.get_bool_report()[x])
        combined_dict[x] = dict_per_ind
    return combined_dict