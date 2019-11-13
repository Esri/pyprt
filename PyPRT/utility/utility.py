# Python utility functions
#import numpy as np

def visualize_PRT_results(models):
    print('\nNumber of generated geometries (= nber of initial shapes):')
    print(len(models))

    for m in models:
        if m:
            geometry_vertices = m.get_vertices()
            rep = m.get_report()

            print()
            print('Initial Shape Index: ' + str(m.get_initial_shape_index()))

            if len(geometry_vertices) > 0:
                print()
                print('Size of the matrix containing the model vertices (with possible duplicates): (' + str(len(geometry_vertices)) + ', 3)')

                # geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
                # print('Size of the matrix containing the model vertices (no duplicates): ' + str(geo_numpy[indices].shape))
                # temp = set()
                # geo_unique = [x for x in geometry_vertices if tuple(x) not in temp and (temp.add(tuple(x)) or True)]
                # print(len(geo_unique))

                print('Size of the matrix containing the model faces: ' + str(len(m.get_faces())))

            if len(rep) > 0 :
                print()
                print('Report of the generated model:')
                print(rep)
        else:
            print('\nError while instanciating the model generator.')
