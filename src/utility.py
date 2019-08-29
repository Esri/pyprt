# Python utility functions
import numpy as np
import copy
# import pyprt
# from arcgis.geometry import Geometry

def collect_initial_shape_indices(models):
    generation_indices = []
    for m in models:
        if generation_indices.count(m.get_initial_shape_index()) == 0:
            generation_indices.append(m.get_initial_shape_index())
    return generation_indices


def visualize_PRT_results(models, printing = False):
    initial_shape_idx = collect_initial_shape_indices(models)

    print("\nNumber of generated geometries (= nber of initial shapes):")
    print(len(initial_shape_idx))

    for m in models:
        if m:
            geometry_vertices = m.get_vertices()
            rep_float = m.get_float_report()
            rep_string = m.get_string_report()
            rep_bool = m.get_bool_report()
            combined_rep = combine_reports(m)

            print()
            print("Initial Shape Index: " + str(m.get_initial_shape_index()))

            if len(geometry_vertices) > 0:
                print()
                geo_numpy = np.array(geometry_vertices)
                print("Size of the matrix containing the model vertices (with possible duplicates): " + str(geo_numpy.shape))

                geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
                print("Size of the matrix containing the model vertices (no duplicates): " + str(geo_numpy[indices].shape))

                print("Size of the matrix containing the model faces: " + str(np.array(m.get_faces()).shape))

            if len(combined_rep) > 0 :
                print()
                print("Report of the generated model:")
                print(combined_rep)

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
    combined_dict.update(model.get_float_report())
    combined_dict.update(model.get_string_report())
    combined_dict.update(model.get_bool_report())
    return combined_dict


def convert_2darcgis_to_pyprt(geometry_2d):
    geo = geometry_2d[0]
    a = geo[:-1]
    b = np.flip(a,axis=0)
    b[:,1] *= -1
    c = np.insert(b, 1, 0, axis=1)
    d = np.reshape(c,(1,c.shape[0]*c.shape[1]))
    return (d.tolist()[0])
