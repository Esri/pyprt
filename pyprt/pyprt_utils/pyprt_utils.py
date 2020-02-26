# Copyright (c) 2012-2020 Esri R&D Center Zurich

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# A copy of the license is available in the repository's LICENSE file.


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
    vertices_as_matrix = []
    for count in range(0, int(len(vertices)/3)):
        vector_per_pt = [vertices[count*3],
                         vertices[count*3+1], vertices[count*3+2]]
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
