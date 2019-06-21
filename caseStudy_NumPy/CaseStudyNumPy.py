import sys, os
sys.path.append(os.path.join(os.getcwd(), "src"))
from utility import visualize_PRT_results, combine_reports, summarize_matrix, summarize_report

SDK_PATH = os.path.join(os.getcwd(), "install", "bin")
sys.path.append(SDK_PATH)

import pyprt

import numpy as np
import itertools
from vispy import app, scene, visuals
from vispy.color import Color, Colormap
from vispy.geometry.meshdata import MeshData

CS_FOLDER = os.getcwd()
def asset_file(filename):
    return os.path.join(CS_FOLDER, "caseStudy_NumPy", filename)


class Canvas(scene.SceneCanvas):
    def __init__(self, geometries_number, generated_data, generated_data_faces , window_range_xmin, window_range_xmax, window_range_ymin, window_range_ymax, window_range_zmin, window_range_zmax):
        scene.SceneCanvas.__init__(self, keys='interactive', size=(800, 550), show=True, title="Visualization of the generated model(s)")

        self.unfreeze()
        self.view = self.central_widget.add_view()
        self.view.camera = 'turntable'
        self.view.camera.set_range((window_range_xmin, window_range_xmax), (window_range_ymin, window_range_ymax), (window_range_zmin, window_range_zmax))
        self.selected_point = None
        scene.visuals.GridLines(parent=self.view.scene)

        for k in range(geometries_number):
            if(generated_data_faces[k] is not None):
                mdata = MeshData(vertices = generated_data[k], faces = generated_data_faces[k])

                rng = np.random.RandomState(0)
                nf = mdata.n_faces
                fcolor = np.ones((nf, 4), dtype=np.float32)
                someColors = [Color('green').RGBA ,Color('yellow').RGBA, Color('blue').RGBA,Color('red').RGBA, Color('white').RGBA,Color('fuchsia').RGBA]

                for l in range(0,nf):
                    k_ind = k
                    if(k >= len(someColors)):
                        k_ind -= len(someColors)
                    fcolor[l] = someColors[k_ind]

                #fcolor[0] = Color('blue').RGBA
                mdata.set_face_colors(fcolor)
                edge_v_index = mdata.get_edges()

                mesh = scene.visuals.Mesh(meshdata = mdata)
                
                mesh.set_gl_state('additive', depth_test=False)
                mlines = scene.visuals.Line(pos=generated_data[k],color='red',connect=edge_v_index)

                self.view.add(mesh)
                self.view.add(mlines)

            s = scene.Markers(pos=generated_data[k])

            self.view.add(s)

        self.freeze()



if __name__ == '__main__':

    VAL = pyprt.print_val(23)
    print("\nTest Function: it should print 23.")
    print(VAL)

    print("\nInitializing PRT.")
    pyprt.initialize_prt(SDK_PATH)

    if(not pyprt.is_prt_initialized()):
        raise Exception("PRT is not initialized")

    initialGeometry = pyprt.Geometry(np.array([0, 0, 0,  0, 0, 2,  1, 0, 1,  1, 0, 0],dtype='f'))
    initialGeometry2 = pyprt.Geometry(np.array([4, 0, 0,  4, 0, 2,  5, 0, 1,  5, 0, 0],dtype='f'))
    rpk = asset_file("simple_rule2019.rpk")
    attrs = ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"]

    mod = pyprt.ModelGenerator([initialGeometry, initialGeometry2])
    generated_mod = mod.generate_model(rpk, attrs)
    all_vertices = []
    all_faces = []

    if(len(generated_mod)>0):
        for model in generated_mod:
            temp = []
            geo = model.get_vertices()
            geo_summarized = summarize_matrix(geo)
            geo_numpy = np.array(geo_summarized)
            geo_numpy_unique, indices = np.unique(np.around(geo_numpy,decimals=3), return_index = True, axis=0)
            for i in indices:
                temp.append(geo_summarized[i]) # to avoid duplicates
            all_vertices.append(temp)
            face_geo = model.get_faces()
            face_summarized = summarize_matrix(face_geo)
            all_faces.append(face_summarized)
            print("\nSize of the matrix containing all the model vertices:")
            print(geo_numpy.shape)
            print(geo_numpy)
            print("\nGenerated Model Faces: ")
            print(face_summarized)
    else:
        print("\nError while instanciating the model generator.")

    print("\nShutdown PRT.")
    pyprt.shutdown_prt()

    # Data
    mat = np.array(all_vertices).copy()
    mat[:, :, 1], mat[:, :, 2] = mat[:, :, 2], mat[:, :, 1].copy()
    mat_f = []

    xmin = np.amin(mat[:,:,0])
    xmax = np.amax(mat[:,:,0])
    ymin = np.amin(mat[:,:,1])
    ymax = np.amax(mat[:,:,1])
    zmin = np.amin(mat[:,:,2])
    zmax = np.amax(mat[:,:,2])

    for k in range(mat.shape[0]):
        mat_faces = []
        for f in all_faces[k]:
            if(len(f) == 3):
                mat_faces.append(f)
            elif(len(f) > 3):
                # in vispy, 3 vertex indices per face
                for new_f in np.array(list(itertools.combinations(f,3))):
                    mat_faces.append(new_f)
            elif(len(f) < 3):
                # repeat one vertex index
                current_ind = 0
                new_f2 = []
                while(len(new_f2) < 3):
                    new_f2.append(f[current_ind])
                    if(current_ind < len(f)-1):
                        current_ind += 1
                    else:
                        while(len(new_f2) < 3):
                            new_f2.append(f[current_ind])
                        current_ind += 1

                mat_faces.append(new_f2)

        mat_f.append(mat_faces)


    win = Canvas(mat.shape[0],mat,np.array(mat_f), xmin, xmax, ymin, ymax, zmin, zmax)
    if sys.flags.interactive != 1:
        app.run()
