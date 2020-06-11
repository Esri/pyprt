/**
 * CityEngine SDK Geometry Encoder for Python
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

#ifdef _WIN32
#	define _CRT_SECURE_NO_WARNINGS
#endif

#include "ModelGenerator.h"
#include "PRTContext.h"
#include "logging.h"
#include "utils.h"

#include "prt/API.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>
#ifdef _WIN32
#	include <direct.h>
#endif

namespace py = pybind11;

namespace {

constexpr const wchar_t* ANNOT_HIDDEN = L"@Hidden";

void initializePRT() {
	auto prt = PRTContext::get(); // this will implicitly construct PRTContext and call prt::init
	if (!prt)
		LOG_ERR << "Failed to initialize PyPRT";
}

bool isPRTInitialized() {
	return (bool)PRTContext::get();
}

void shutdownPRT() {
	PRTContext::shutdown();
}

py::dict getRuleAttributes(const prt::RuleFileInfo* ruleFileInfo) {
	auto ruleAttrs = py::dict();

	for (size_t i = 0; i < ruleFileInfo->getNumAttributes(); i++) {
		bool hidden = false;
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo->getAttribute(i);

		const std::wstring fullName(attr->getName());
		if (fullName.find(L"Default$") != 0)
			continue;
		const std::wstring name = fullName.substr(8);
		const prt::AnnotationArgumentType valueType = attr->getReturnType();
		py::str type;

		for (size_t f = 0; f < attr->getNumAnnotations(); f++) {
			if (!(std::wcscmp(attr->getAnnotation(f)->getName(), ANNOT_HIDDEN))) {
				hidden = true;
				break;
			}
		}

		if (!hidden) {
			if (valueType == prt::AAT_STR)
				type = "string";
			else if (valueType == prt::AAT_BOOL)
				type = "bool";
			else if (valueType == prt::AAT_FLOAT)
				type = "float";
			else if (valueType == prt::AAT_STR_ARRAY)
				type = "string[]";
			else if (valueType == prt::AAT_BOOL_ARRAY)
				type = "bool[]";
			else if (valueType == prt::AAT_FLOAT_ARRAY)
				type = "float[]";
			else
				type = "UNKNOWN_VALUE_TYPE";

			ruleAttrs[py::cast(name)] = type;
		}
	}

	return ruleAttrs;
}

py::dict inspectRPK(const std::filesystem::path& rulePackagePath) {
	pcu::ResolveMapPtr resolveMap;

	if (!std::filesystem::exists(rulePackagePath) || !pcu::getResolveMap(rulePackagePath, &resolveMap)) {
		LOG_ERR << "invalid rule package path";
		return py::dict();
	}

	std::wstring ruleFile = pcu::getRuleFileEntry(resolveMap.get());

	const wchar_t* ruleFileURI = resolveMap->getString(ruleFile.c_str());
	if (ruleFileURI == nullptr) {
		LOG_ERR << "could not find rule file URI in resolve map of rule package " << rulePackagePath;
		return py::dict();
	}

	prt::Status infoStatus = prt::STATUS_UNSPECIFIED_ERROR;
	pcu::RuleFileInfoUPtr info(prt::createRuleFileInfo(ruleFileURI, nullptr, &infoStatus));
	if (!info || infoStatus != prt::STATUS_OK) {
		LOG_ERR << "could not get rule file info from rule file " << ruleFile;
		return py::dict();
	}

	py::dict ruleAttrs = getRuleAttributes(info.get());

	return ruleAttrs;
}

} // namespace

using namespace pybind11::literals;

PYBIND11_MODULE(pyprt, m) {
	const char* docInit = "Initialization of PRT. PyPRT functionalities are blocked until the initialization is done.";
	const char* docIsInit = R"mydelimiter(
        is_prt_initialized() -> bool

        This function returns *True* if PRT is initialized, *False* otherwise.

        :Returns:
            bool
    )mydelimiter";
	const char* docShutdown = "Shutdown of PRT. The PRT initialization process can be done only once per "
	                          "session/script. Thus, ``initialize_prt()`` cannot be called after ``shutdown_prt()``.";
	const char* docInspectRPK = R"mydelimiter(
        inspect_rpk(rule_package_path) -> dict

        This function returns the CGA rule attributes name and value type for the specified rule package path.

        :Returns:
            dict
    )mydelimiter";
	const char* docIs = R"mydelimiter(
        __init__(*args, **kwargs)

        The initial shape corresponds to the geometry on which the CGA rule will be applied.
        )mydelimiter";
	const char* docIsInitV = R"mydelimiter(
        1. **__init__** (*vert_coordinates*)

        Constructs an InitialShape with one polygon by accepting a list of direct vertex coordinates. The 
        vertex order is expected to be counter-clockwise.

        :Parameters:
            **vert_coordinates** -- List[float]
        :Example: ``shape1 = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0])``
        )mydelimiter";
	const char* docIsInitVI = R"mydelimiter(
        2. **__init__** (*vert_coordinates*, *face_indices*, *face_count*)

        Constructs an InitialShape by accepting a list of direct vertex coordinates, a list of the vertex
        indices for each faces and a list of the indices count per face. The vertex order is expected to 
        be counter-clockwise.

        :Parameters:
            - **vert_coordinates** -- List[float]
            - **face_indices** -- List[int]
            - **face_count** -- List[int]
        :Example: ``shape2 = pyprt.InitialShape([0, 0, 0, 0, 0, 10, 10, 0, 10, 10, 0, 0], [0, 1, 2, 3], [4])``
        )mydelimiter";
	const char* docIsInitP = R"mydelimiter(
        3. **__init__** (*init_shape_path*)

        Constructs an InitialShape by accepting the path to a shape file. This can be an OBJ file, Collada, etc. A list
		of supported file formats can be found at `PRT geometry encoders
		<https://esri.github.io/cityengine-sdk/html/esri_prt_codecs.html>`_.

        :Parameters:
            **initialShapePath** -- str
        :Example: ``shape3 = pyprt.InitialShape(os.path.join(os.getcwd(), 'myInitialShape.obj'))``
        )mydelimiter";
	const char* docIsGetV = R"mydelimiter(
        get_vertex_count() -> int

        Returns the number of vertex coordinates of the initial shape, only if the InitialShape has been 
        initialized from a list of vertex coordinates.

        :Returns:
            int
        )mydelimiter";
	const char* docIsGetI = R"mydelimiter(
        get_index_count() -> int

        Returns the length of the vector containing the vertex indices of the initial shape, only if the 
        InitialShape has been initialized from a list of vertex coordinates.

        :Returns:
            int
        )mydelimiter";
	const char* docIsGetF = R"mydelimiter(
        get_face_counts_count() -> int

        Returns the number of faces of the initial shape, only if the InitialShape has been initialized from a
        list of vertex coordinates.

        :Returns:
            int
        )mydelimiter";
	const char* docIsGetP = R"mydelimiter(
        get_path() -> str

        Returns the initial shape file path, if the InitialShape has been initialized from a file. Empty otherwise.

        :Returns:
            str
        )mydelimiter";
	const char* docMg = "The ModelGenerator class will host the data required to procedurally generate the 3D model on "
	                    "a given initial shape.";
	const char* docMgInit = R"mydelimiter(
        __init__(init_shapes)

        The ModelGenerator constructor takes a list of InitialShape instances as parameter.

        :Parameters:
            **init_shapes** -- List[InitialShape]

        )mydelimiter";
	const char* docMgGen = R"mydelimiter(
        generate_model(*args, **kwargs) -> List[GeneratedModel]

        This function does the procedural generation of the models. It outputs a list of GeneratedModel instances. 
        You need to provide one shape attribute dictionary per initial shape or one dictionary that will be applied 
        to all initial shapes. The shape attribute dictionary only contains either string, float or bool values, **except** the 
        ``'seed'`` value, which has to be an integer (default value equals to *0*). The ``'shapeName'`` is 
        another non-mandatory entry (default value equals to *"InitialShape"*). In addition to the seed and the shape name keys, 
        the shape attribute dictionary will contain the CGA input attributes specific to the CGA file you are using (use the 
        *inspect_rpk* function to know these input attributes). Concerning the encoder, you can use the 
        ``'com.esri.pyprt.PyEncoder'`` or any other geometry encoder. The PyEncoder has two options: 
        ``'emitGeometry'`` and ``'emitReport'`` whose value is a boolean. The complete list of the other geometry 
        encoders can be found `here <https://esri.github.io/cityengine-sdk/html/esri_prt_codecs.html>`__. In
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
	const char* docMgGen2 = R"mydelimiter(
        This overloaded *generate_model* function can only be used once the previous *ModelGenerator::generate_model* method has been 
        called. It is useful to specify different shape attributes but use the same CGA rule package on the
        same initial shapes, the same encoder and encoder options.

        :Parameters:
            **shape_attributes** -- List[dict]

        :Returns:
            List[GeneratedModel]
        :Example: ``models2 = m.generate_model([attrs3, attrs4])``
        )mydelimiter";
	const char* docGm = "The GeneratedModel instance contains the generated 3D geometry. This class is only employed "
	                    "if the *com.esri.pyprt.PyEncoder* encoder is used in the ModelGenerator instance.";
	const char* docGmGetInd = R"mydelimiter(
        get_initial_shape_index() -> int

        Returns the index of the initial shape on which the generated geometry has been built. The 
        :py:class:`ModelGenerator class <pyprt.pyprt.bin.pyprt.ModelGenerator>` is instantiated by specifying a list of 
        InitialShape instances. This index indicates the corresponding InitialShape instance of that list.

        :Returns:
            int
        )mydelimiter";
	const char* docGmGetV = R"mydelimiter(
        get_vertices() -> List[float]

        Returns the generated 3D geometry vertex coordinates as a series of (x, y, z) triplets. Its size is 3 x the 
        number of vertices. If the ``'emitGeometry'`` entry of the encoder options dictionary has been set to *False*, 
        this function returns an empty vector.

        :Returns:
            List[float]
        )mydelimiter";
	const char* docGmGetI = R"mydelimiter(
        get_indices() -> List[int]
        
        Returns the vertex indices of the generated 3D geometry, for all faces. If the ``'emitGeometry'`` entry of the encoder
        options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[int]
        )mydelimiter";
	const char* docGmGetF = R"mydelimiter(
        get_faces() -> List[int]

        Returns the vertex indices count per face of the generated 3D geometry. If the ``'emitGeometry'`` entry of the
        encoder options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[int]
        )mydelimiter";
	const char* docGmGetR = R"mydelimiter(
        get_report() -> dict

        Returns the CGA report of the generated 3D geometry. This report dictionary is empty if the CGA rule
        file employed does not output any report or if the ``'emitReport'`` entry of the encoder options
        dictionary has been set to *False*.

        :Returns:
            dict
        )mydelimiter";

	py::options options;
	options.disable_function_signatures();

	py::bind_vector<std::vector<GeneratedModel>>(m, "GeneratedModelVector", py::module_local(false));

	m.def("initialize_prt", &initializePRT, docInit);
	m.def("is_prt_initialized", &isPRTInitialized, docIsInit);
	m.def("shutdown_prt", &shutdownPRT, docShutdown);
	m.def("inspect_rpk", &inspectRPK, py::arg("rulePackagePath"), docInspectRPK);

	py::class_<InitialShape>(m, "InitialShape", docIs)
	        .def(py::init<const std::vector<double>&>(), py::arg("vertCoordinates"), docIsInitV)
	        .def(py::init<const std::vector<double>&, const std::vector<uint32_t>&, const std::vector<uint32_t>&>(),
	             py::arg("vertCoordinates"), py::arg("faceVertIndices"), py::arg("faceVertCount"), docIsInitVI)
	        .def(py::init<const std::string&>(), py::arg("initialShapePath"), docIsInitP)
	        .def("get_vertex_count", &InitialShape::getVertexCount, docIsGetV)
	        .def("get_index_count", &InitialShape::getIndexCount, docIsGetI)
	        .def("get_face_counts_count", &InitialShape::getFaceCountsCount, docIsGetF)
	        .def("get_path", &InitialShape::getPath, docIsGetP);

	py::class_<ModelGenerator>(m, "ModelGenerator", docMg)
	        .def(py::init<const std::vector<InitialShape>&>(), py::arg("initialShapes"), docMgInit)
	        .def("generate_model", &ModelGenerator::generateModel, py::arg("shapeAttributes"),
	             py::arg("rulePackagePath"), py::arg("geometryEncoderName"), py::arg("geometryEncoderOptions"),
	             docMgGen)
	        .def("generate_model", &ModelGenerator::generateAnotherModel, py::arg("shapeAttributes"), docMgGen2);

	py::class_<GeneratedModel>(m, "GeneratedModel", docGm)
	        .def("get_initial_shape_index", &GeneratedModel::getInitialShapeIndex, docGmGetInd)
	        .def("get_vertices", &GeneratedModel::getVertices, docGmGetV)
	        .def("get_indices", &GeneratedModel::getIndices, docGmGetI)
	        .def("get_faces", &GeneratedModel::getFaces, docGmGetF)
	        .def("get_report", &GeneratedModel::getReport, docGmGetR);

	py::class_<std::filesystem::path>(m, "Path").def(py::init<std::string>());
	py::implicitly_convertible<std::string, std::filesystem::path>();

} // PYBIND11_MODULE
