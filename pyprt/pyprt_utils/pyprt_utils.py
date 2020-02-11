def visualize_prt_results(models):
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
                print('Size of the matrix containing the model vertices (with possible duplicates): (' + str(int(len(geometry_vertices)/3)) + ', 3)')
                print('Size of the matrix containing the model faces: ' + str(len(m.get_faces())))

            if len(rep) > 0 :
                print()
                print('Report of the generated model:')
                print(rep)
        else:
            print('\nError while instanciating the model generator.')

def vertices_vector_to_matrix(vertices):
    vertices_as_matrix = []
    for count in range(0,int(len(vertices)/3)):
        vector_per_pt = [vertices[count*3], vertices[count*3+1], vertices[count*3+2]]
        vertices_as_matrix.append(vector_per_pt)
    return vertices_as_matrix

def faces_indices_vectors_to_matrix(indices, faces):
    faces_as_matrix = []
    offset = 0
    for f in faces:
        ind_per_face = indices[offset:offset+f]
        offset += f
        faces_as_matrix.append(ind_per_face)
    return faces_as_matrix
