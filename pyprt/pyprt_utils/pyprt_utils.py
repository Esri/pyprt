def visualize_prt_results(models):
    """visualize_prt_results(models)
    This helper function is used to output the geometry and report information of a 
    vector of GeneratedModel.

    Parameters:
        models: List[GeneratedModel]
    """
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
                print('Size of the model vertices vector: ' +
                      str(len(geometry_vertices)))
                print('Number of model vertices: ' +
                      str(int(len(geometry_vertices)/3)))
                print('Size of the model faces vector: ' +
                      str(len(m.get_faces())))

            if len(rep) > 0:
                print()
                print('Report of the generated model:')
                print(rep)
        else:
            print('\nError while instanciating the model generator.')


def vertices_vector_to_matrix(vertices):
    """vertices_vector_to_matrix(vertices) -> List[List[float]]
    PyPRT outputs the GeneratedModel vertex coordinates as a 1D vector. The vector 
    contains the x, y, z coordinates of all the vertices. This function converts the 
    vertex vector into a vector of N vertex coordinates vectors (with N, the number 
    of geometry vertices).

    Parameters:
        vertices: List[float]

    Returns:
        List[List[float]]

    Example:
        ``[[-10.0, 0.0, 10.0], [-10.0, 0.0, 0.0], [10.0, 0.0, 0.0], [10.0, 0.0, 10.0]] 
        = vertices_vector_to_matrix([-10.0, 0.0, 10.0, -10.0, 0.0, 0.0, 10.0, 0.0, 0.0, 
        10.0, 0.0, 10.0])``
    """
    vertices_as_matrix = []
    for count in range(0, int(len(vertices)/3)):
        vector_per_pt = [vertices[count*3],
                         vertices[count*3+1], vertices[count*3+2]]
        vertices_as_matrix.append(vector_per_pt)
    return vertices_as_matrix


def faces_indices_vectors_to_matrix(indices, faces):
    """faces_indices_vectors_to_matrix(indices, faces) -> List[List[int]]
    PyPRT outputs the GeneratedModel face information as a vector of vertex indices 
    and a vector of face indices count. This function converts these two vectors into 
    one vector of vectors containing the vertex indices per face.

    Parameters:
        indices: List[int]
        faces: List[int]

    Returns:
        List[List[int]]

    Example:
        ``[[1, 0, 3, 2], [4, 5, 6, 7], [0, 1, 5, 4], [1, 2, 6, 5], [2, 3, 7, 6], [3, 0, 4, 7]] 
        = faces_indices_vectors_to_matrix(([1, 0, 3, 2, 4, 5, 6, 7, 0, 1, 5, 4, 1, 2, 6, 5, 
        2, 3, 7, 6, 3, 0, 4, 7],[4, 4, 4, 4, 4, 4]))``
    """
    faces_as_matrix = []
    offset = 0
    for f in faces:
        ind_per_face = indices[offset:offset+f]
        offset += f
        faces_as_matrix.append(ind_per_face)
    return faces_as_matrix
