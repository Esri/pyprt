/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2020 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * A copy of the license is available in the repository's LICENSE file.
 */

#pragma once

namespace doc {

constexpr const char* Init =
        "Initialization of PRT. PyPRT functionalities are blocked until the initialization is done.";

constexpr const char* IsInit = R"mydelimiter(
        is_prt_initialized() -> bool

        This function returns *True* if PRT is initialized, *False* otherwise.

        :Returns:
            bool
    )mydelimiter";

constexpr const char* Shutdown =
        "Shutdown of PRT. The PRT initialization process can be done only once per "
        "session/script. Thus, ``initialize_prt()`` cannot be called after ``shutdown_prt()``.";

constexpr const char* InspectRPKDeprecated = R"mydelimiter(
        inspect_rpk(rule_package_path) -> dict
        
        Deprecated: use ``get_rpk_attributes_info(rule_package_path)`` instead.

        This function returns the CGA rule attributes name and value type for the specified rule package path.

        :Returns:
            dict
    )mydelimiter";

constexpr const char* GetRPKInfo = R"mydelimiter(
        get_rpk_attributes_info(rule_package_path) -> dict

        This function returns the CGA rule attributes name and value type for the specified rule package path as 
        well as a list of the attributes annotations (annotation name, key(s) and value(s)). In case of an unnamed 
        annotation parameter, its key is equal to ``'#NULL#'``, which can be read using the ``pyprt.NO_KEY`` constant.

        :Returns:
            dict
    )mydelimiter";

constexpr const char* Is = R"mydelimiter(
        __init__(*args, **kwargs)

        The initial shape corresponds to the geometry on which the CGA rule will be applied.
        )mydelimiter";

constexpr const char* IsInitV = R"mydelimiter(
        1. **__init__** (*vert_coordinates*)

        Constructs an InitialShape with one polygon by accepting a list of direct vertex coordinates. The
        vertex order is expected to be counter-clockwise.

        :Parameters:
            **vert_coordinates** -- List[float]
        :Example: ``shape1 = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0])``
        )mydelimiter";

constexpr const char* IsInitVI = R"mydelimiter(
        2. **__init__** (*vert_coordinates*, *face_indices*, *face_count*, *holes*)

        Constructs an InitialShape by accepting a list of direct vertex coordinates, a list of the vertex
        indices for each faces and a list of the indices count per face. The vertex order is expected to 
        be counter-clockwise. The last parameter, *holes*, is optional and allows defining holes polygons in faces.
        It is a list of lists, that assign hole-faces to faces. It follows this structure:

		``[[index-of-face1-with-holes, index-of-hole1-in-face1, index-of-hole2-in-face1,...], ..., [index-of-faceN-with-holes, index-of-hole1-in-faceN, index-of-hole2-in-faceN, ...]]``
        
        Holes must have the opposite vertex-ordering as the encircling face.

        :Parameters:
            - **vert_coordinates** -- List[float]
            - **face_indices** -- List[int]
            - **face_count** -- List[int]
            - **holes** -- List[List[int]]
        :Examples: ``shape_without_holes =``
							``pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0], [0, 1, 2, 3], [4])``
				   ``shape_with_hole =``
							``pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0, 2, 0, 2, 8, 0, 8, 2, 0, 8], [0, 1, 2, 3, 4, 5, 6], [4, 3], [[0, 1]])``
        )mydelimiter";

constexpr const char* IsInitP = R"mydelimiter(
        3. **__init__** (*init_shape_path*)

        Constructs an InitialShape by accepting the path to a shape file. This can be an OBJ file, Collada, etc.
        A list of supported file formats can be found at `PRT geometry encoders <https://esri.github.io/cityengine-sdk/html/esri_prt_codecs.html>`_.

        :Parameters:
            **initial_shape_path** -- str
        :Example: ``shape3 = pyprt.InitialShape(os.path.join(os.getcwd(), 'myInitialShape.obj'))``
        )mydelimiter";

constexpr const char* IsGetV = R"mydelimiter(
        get_vertex_count() -> int

        Returns the number of vertex coordinates of the initial shape, only if the :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` has been
        initialized from a list of vertex coordinates.

        :Returns:
            int
        )mydelimiter";

constexpr const char* IsGetI = R"mydelimiter(
        get_index_count() -> int

        Returns the length of the vector containing the vertex indices of the initial shape, only if the
        :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` has been initialized from a list of vertex coordinates.

        :Returns:
            int
        )mydelimiter";

constexpr const char* IsGetF = R"mydelimiter(
        get_face_counts_count() -> int

        Returns the number of faces of the initial shape, only if the :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` has been initialized from a
        list of vertex coordinates.

        :Returns:
            int
        )mydelimiter";

constexpr const char* IsGetP = R"mydelimiter(
        get_path() -> str

        Returns the initial shape file path, if the :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` has been initialized from a file. Empty otherwise.

        :Returns:
            str
        )mydelimiter";

constexpr const char* Mg =
        "The ModelGenerator class will host the data required to procedurally generate the 3D model on "
        "a given initial shape.";

constexpr const char* MgInit = R"mydelimiter(
        __init__(init_shapes)

        The ModelGenerator constructor takes a list of :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` instances as parameter.

        :Parameters:
            **init_shapes** -- List[InitialShape]

        )mydelimiter";

constexpr const char* MgGen = R"mydelimiter(
        generate_model(*args, **kwargs) -> List[GeneratedModel]

        This function does the procedural generation of the models. It outputs a list of :py:class:`GeneratedModel <pyprt.pyprt.bin.pyprt.GeneratedModel>` instances.
        You need to provide one shape attribute dictionary per initial shape or one dictionary that will be applied
        to all initial shapes. The shape attribute dictionary only contains either string, float or bool values, **except** the
        ``'seed'`` value, which has to be an integer (default value equals to *0*). The ``'shapeName'`` is
        another non-mandatory entry (default value equals to *"InitialShape"*). In addition to the seed and the shape name keys,
        the shape attribute dictionary will contain the CGA input attributes specific to the CGA file you are using (use the
        ``get_rpk_attributes_info`` function to know these input attributes). Concerning the encoder, you can use the
        ``'com.esri.pyprt.PyEncoder'`` or any other geometry encoder. The PyEncoder has two options:
        ``'emitGeometry'`` and ``'emitReport'`` whose value is a boolean. The complete list of the other geometry
        encoders can be found `here <https://esri.github.io/esri-cityengine-sdk/html/esri_prt_codecs.html>`__. In
        case you are using another geometry encoder than the PyEncoder, you can add an ``'outputPath'`` entry to
        the shape attribute dictionary to specify where the generated 3D geometry will be outputted. In this case,
        the return value of this *generate_model* function will be an empty list.

        :Parameters:
            - **shape_attributes** -- List[dict]
            - **rule_package_path** -- str
            - **geometry_encoder** -- str
            - **encoder_options** -- dict

        :Returns:
            List[GeneratedModel]
        :Example:
            ``m = pyprt.ModelGenerator([shape1, shape2])``

            ``rpk = os.path.join(os.getcwd(), 'extrusion_rule.rpk')``

            ``attrs1 = {'shapeName': 'myShape1', 'seed': 555, 'minBuildingHeight': 30.0}``

            ``attrs2 = {'shapeName': 'myShape2', 'seed': 777, 'minBuildingHeight': 25.0}``

            ``models1 = m.generate_model([attrs1, attrs2], rpk, 'com.esri.pyprt.PyEncoder', {'emitReport': True, 'emitGeometry': True})``
        )mydelimiter";

constexpr const char* Gm =
        "The GeneratedModel instance contains the generated 3D geometry. This class is only employed "
        "if the *com.esri.pyprt.PyEncoder* encoder is used in the :py:class:`ModelGenerator <pyprt.pyprt.bin.pyprt.ModelGenerator>` instance.";

constexpr const char* GmGetInd = R"mydelimiter(
        get_initial_shape_index() -> int

        Returns the index of the initial shape on which the generated geometry has been built. The
        :py:class:`ModelGenerator <pyprt.pyprt.bin.pyprt.ModelGenerator>` class is instantiated by specifying a list of
        :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` instances. This index indicates the corresponding 
        :py:class:`InitialShape <pyprt.pyprt.bin.pyprt.InitialShape>` instance of that list.

        :Returns:
            int
        )mydelimiter";

constexpr const char* GmGetV = R"mydelimiter(
        get_vertices() -> List[float]

        Returns the generated 3D geometry vertex coordinates as a series of (x, y, z) triplets. Its size is 3 x the
        number of vertices. If the ``'emitGeometry'`` entry of the encoder options dictionary has been set to *False*,
        this function returns an empty vector.

        :Returns:
            List[float]
        )mydelimiter";

constexpr const char* GmGetI = R"mydelimiter(
        get_indices() -> List[int]

        Returns the vertex indices of the generated 3D geometry, for all faces. If the ``'emitGeometry'`` entry of the encoder
        options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[int]
        )mydelimiter";

constexpr const char* GmGetF = R"mydelimiter(
        get_faces() -> List[int]

        Returns the vertex indices count per face of the generated 3D geometry. If the ``'emitGeometry'`` entry of the
        encoder options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[int]
        )mydelimiter";

constexpr const char* GmGetR = R"mydelimiter(
        get_report() -> dict

        Returns the CGA report of the generated 3D geometry. This report dictionary is empty if the CGA rule
        file employed does not output any report or if the ``'emitReport'`` entry of the encoder options
        dictionary has been set to *False*.

        :Returns:
            dict
        )mydelimiter";

constexpr const char* GmGetP = R"mydelimiter(
        get_cga_prints() -> str

        Returns a string with all the CGA print outputs of this model.

        :Returns:
            str
        )mydelimiter";

constexpr const char* GmGetE = R"mydelimiter(
        get_cga_errors() -> List[str]

        Returns a list of the CGA and asset errors messages of this model. The asset error messages additionally 
        contain the key and URI of the asset.

        :Returns:
            List[str]
        )mydelimiter";

} // namespace doc
