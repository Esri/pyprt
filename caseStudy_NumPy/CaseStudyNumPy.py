import sys, os

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
    def __init__(self, generated_data, generated_data_faces , window_range_xmin, window_range_xmax, window_range_ymin, window_range_ymax, window_range_zmin, window_range_zmax):
        scene.SceneCanvas.__init__(self, keys='interactive', size=(800, 550), show=True, title="Visualization of the generated model(s)")

        self.unfreeze()
        self.view = self.central_widget.add_view()
        self.view.camera = 'turntable'
        self.view.camera.set_range((window_range_xmin, window_range_xmax), (window_range_ymin, window_range_ymax), (window_range_zmin, window_range_zmax))
        self.selected_point = None
        scene.visuals.GridLines(parent=self.view.scene)

        if(generated_data_faces is not None):
            mdata = MeshData(vertices = generated_data, faces = generated_data_faces)

            rng = np.random.RandomState(0)
            nf = mdata.n_faces
            fcolor = np.ones((nf, 4), dtype=np.float32)
            someColors = [Color('green').RGBA, Color('green').RGBA ,Color('yellow').RGBA, Color('yellow').RGBA, Color('blue').RGBA, Color('blue').RGBA,Color('red').RGBA, Color('red').RGBA, Color('white').RGBA, Color('white').RGBA,Color('fuchsia').RGBA, Color('fuchsia').RGBA]

            for l in range(0,nf):
                fcolor[l] = Color('green').RGBA #someColors[l]

            #fcolor[0] = Color('blue').RGBA
            mdata.set_face_colors(fcolor)
            edge_v_index = mdata.get_edges()

            mesh = scene.visuals.Mesh(meshdata = mdata)
            
            mesh.set_gl_state('additive', depth_test=False)
            mlines = scene.visuals.Line(pos=generated_data,color='red',connect=edge_v_index)

            self.view.add(mesh)
            self.view.add(mlines)

        s = scene.Markers(pos=generated_data)

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

    v = np.array([0, 0, 0,  0, 0, 2,  1, 0, 1,  1, 0, 0],dtype='f')
    v2 = np.array([4, 0, 0,  4, 0, 2,  5, 0, 1,  5, 0, 0],dtype='f')
    initialGeometry = pyprt.CustomGeometry(v)
    initialGeometry2 = pyprt.CustomGeometry(v2)

    mod = pyprt.ModelGenerator([initialGeometry], asset_file("simple_rule2019.rpk"), ["ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint"], ["baseName:string=theModel"])

    if(mod.generate_model()):
        geo = mod.get_model_geometry()
        geo_numpy = np.array(geo)
        face_geo = mod.get_model_faces_geometry()
        print("\nSize of the matrix containing all the model vertices:")
        print(geo_numpy.shape)
        print(geo_numpy)
        print("\nGenerated Model Faces: ")
        print(face_geo)
    else:
        print("\nError while instanciating the model generator.")


    print("\nShutdown PRT.")
    pyprt.shutdown_prt()


    # Data
    mat = geo_numpy[0].copy()
    #mat[:, 1], mat[:, 2] = mat[:, 2], mat[:, 1].copy()
    mat_f = []

    xmin = np.amin(mat[:,0])
    xmax = np.amax(mat[:,0])
    ymin = np.amin(mat[:,1])
    ymax = np.amax(mat[:,1])
    zmin = np.amin(mat[:,2])
    zmax = np.amax(mat[:,2])

    mat_faces = []
    for f in face_geo[0]:
        if(len(f) == 3):
            mat_faces.append(f)
        elif(len(f) > 3):
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

    win = Canvas(mat,np.array(mat_faces), xmin, xmax, ymin, ymax, zmin, zmax)
    if sys.flags.interactive != 1:
        app.run()
