# Python utility functions
import numpy as np
import copy

def collect_initial_shape_indices(report_or_matrix):
    generation_indices = []
    summary_report_dicts = []
    for x in report_or_matrix:
        if generation_indices.count(x[0]) == 0:
            generation_indices.append(x[0])
            summary_report_dicts.append({})
    return generation_indices, summary_report_dicts


def summarize_report1shape(report):
    summary_dict = {}
    for x in report:
        if len(x[2]) > 0:
            for y in x[2]:
                if y not in summary_dict:
                    summary_dict[y] = x[2][y]
                elif y in summary_dict and isinstance(x[2][y], float):
                    summary_dict[y] += x[2][y] 
    return summary_dict

def summarize_report(report):
    generation_indices = []
    summary_dicts = []
    for x in report:
        if generation_indices.count(x[0]) == 0:
            generation_indices.append(x[0])
            summary_dicts.append(x[2])
        else:
            ind = generation_indices.index(x[0])
            if len(x[2]) > 0:
                for y in x[2]:
                    if y not in summary_dicts[ind]:
                        summary_dicts[ind][y] = x[2][y]
                    elif y in summary_dicts[ind] and isinstance(x[2][y], float):
                        summary_dicts[ind][y] += x[2][y]
    return summary_dicts

def summarize_report_ind(report, indices, summary_dicts):
    summary_to_return = copy.deepcopy(summary_dicts)
    for x in report:
        if indices.count(x[0]) == 0:
            print("Non existing initial shape index.")
        else:
            ind = indices.index(x[0])
            if len(x[2]) > 0:
                for y in x[2]:
                    if y not in summary_to_return[ind]:
                        summary_to_return[ind][y] = x[2][y]
                    elif y in summary_to_return[ind] and isinstance(x[2][y], float):
                        summary_to_return[ind][y] += x[2][y]
    return summary_to_return


def summarize_matrix1shape(vertices_matrix):
    gathered_matrix = []
    for x in vertices_matrix:
        gathered_matrix.extend(x[2])
    return gathered_matrix


def summarize_matrix(vertices_matrix):
    generation_indices = []
    summary_matrices = []
    for x in vertices_matrix:
        gathered_matrix = []
        if generation_indices.count(x[0]) == 0:
            generation_indices.append(x[0])
            gathered_matrix.extend(x[2])
            summary_matrices.append(gathered_matrix)
        else:
            ind = generation_indices.index(x[0])
            summary_matrices[ind].extend(x[2])
    return summary_matrices


def visualize_PRT_results(model, printing = False):
    if model:
        geometry_vertices = model.get_vertices()
        initial_shape_idx, summary_rep_dict = collect_initial_shape_indices(geometry_vertices)
        geo_summarized = summarize_matrix(geometry_vertices)

        print("\nNumber of generated geometries (= nber of initial shapes):")
        print(len(initial_shape_idx))

        rep_float = model.get_float_report()
        rep_string = model.get_string_report()
        rep_bool = model.get_bool_report()
        sum_rep_float = summarize_report_ind(rep_float, initial_shape_idx, copy.deepcopy(summary_rep_dict))
        sum_rep_string = summarize_report_ind(rep_string, initial_shape_idx, copy.deepcopy(summary_rep_dict))
        sum_rep_bool = summarize_report_ind(rep_bool, initial_shape_idx, copy.deepcopy(summary_rep_dict))

        if printing :
            print("Reports of the generated model (floats report, strings report, bools report):")
            print(rep_float)
            print(rep_string)
            print(rep_bool)

        print()
        for i in range(0,len(geo_summarized)):
            print("Initial Shape Index: " + str(initial_shape_idx[i]))

            geo_numpy = np.array(geo_summarized[i])
            geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
            print("Size of the matrix containing the model vertices (no duplicates): " + str(geo_numpy[indices].shape))

            print("Summarized Reports per generated shape over all subshapes:")
            if len(sum_rep_float[i]) > 0: 
                print(sum_rep_float[i])
            if len(sum_rep_string[i]) > 0: 
                print(sum_rep_string[i])
            if len(sum_rep_bool[i]) > 0: 
                print(sum_rep_bool[i])
    else:
        print("\nError while instanciating the model generator.")



def combine_report1shape(model):
    report = {}
    for x in model.get_float_report():
        if len(x[2]) > 0:
            report.update(x[2])
    for x in model.get_string_report():
        if len(x[2]) > 0:
            report.update(x[2])
    for x in model.get_bool_report():
        if len(x[2]) > 0:
            report.update(x[2])
    return report


def combine_reports(model):
    report = []
    for x in model.get_float_report():
        if len(x[2]) > 0:
            report.append(x)
    for x in model.get_string_report():
        if len(x[2]) > 0:
            report.append(x)
    for x in model.get_bool_report():
        if len(x[2]) > 0:
            report.append(x)
    return report