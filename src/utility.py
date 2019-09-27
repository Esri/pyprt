# Python utility functions
#import numpy as np

def visualize_PRT_results(models, printing = False):
    print("\nNumber of generated geometries (= nber of initial shapes):")
    print(len(models))

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
                print("Size of the matrix containing the model vertices (with possible duplicates): (" + str(len(geometry_vertices)) + ", 3)")

                # geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
                # print("Size of the matrix containing the model vertices (no duplicates): " + str(geo_numpy[indices].shape))
                # temp = set()
                # geo_unique = [x for x in geometry_vertices if tuple(x) not in temp and (temp.add(tuple(x)) or True)]
                # print(len(geo_unique))

                print("Size of the matrix containing the model faces: " + str(len(m.get_faces())))

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
