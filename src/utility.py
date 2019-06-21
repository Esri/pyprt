# Python utility functions
import numpy as np

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


def visualize_PRT_results(models, printing = False):
    if len(models) > 0:
        print("\nNumber of generated geometries (= nber of initial shapes):")
        print(len(models))

        for model in models:

            geometry_vertices = model.get_vertices()
            geo_summarized = summarize_matrix(geometry_vertices)
            geo_numpy = np.array(geo_summarized)
            print("Size of the matrix containing the model vertices (no duplicates):")
            geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
            print(geo_numpy[indices].shape)

            rep_float = model.get_float_report()
            rep_string = model.get_string_report()
            rep_bool = model.get_bool_report()
            if(printing):
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


def combine_reports(model):
    report = {}
    for z in model.get_float_report().keys():
        r1 = model.get_float_report()[z]
        r1.update(model.get_string_report()[z])
        r1.update(model.get_bool_report()[z])
        report[z] = r1
    return report