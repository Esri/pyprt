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

#ifdef _WIN32
#	define _CRT_SECURE_NO_WARNINGS
#endif

#include "ModelGenerator.h"
#include "PRTContext.h"
#include "doc.h"
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

PYBIND11_MODULE(pyprt, m) {

	py::options options;
	options.disable_function_signatures();

	py::bind_vector<std::vector<GeneratedModel>>(m, "GeneratedModelVector", py::module_local(false));

	m.def("initialize_prt", &initializePRT, doc::Init);
	m.def("is_prt_initialized", &isPRTInitialized, doc::IsInit);
	m.def("shutdown_prt", &shutdownPRT, doc::Shutdown);
	m.def("inspect_rpk", &inspectRPK, py::arg("rulePackagePath"), doc::InspectRPK);

	py::class_<InitialShape>(m, "InitialShape", doc::Is)
	        .def(py::init<const std::vector<double>&>(), py::arg("vertCoordinates"), doc::IsInitV)
	        .def(py::init<const std::vector<double>&, const std::vector<uint32_t>&, const std::vector<uint32_t>&,
	                      const HoleIndices&>(),
	             py::arg("vertCoordinates"), py::arg("faceVertIndices"), py::arg("faceVertCount"),
	             py::arg("holes") = std::vector<std::vector<uint32_t>>(), doc::IsInitVI)
	        .def(py::init<const std::string&>(), py::arg("initialShapePath"), doc::IsInitP)
	        .def("get_vertex_count", &InitialShape::getVertexCount, doc::IsGetV)
	        .def("get_index_count", &InitialShape::getIndexCount, doc::IsGetI)
	        .def("get_face_counts_count", &InitialShape::getFaceCountsCount, doc::IsGetF)
	        .def("get_path", &InitialShape::getPath, doc::IsGetP);

	py::class_<ModelGenerator>(m, "ModelGenerator", doc::Mg)
	        .def(py::init<const std::vector<InitialShape>&>(), py::arg("initialShapes"), doc::MgInit)
	        .def("generate_model", &ModelGenerator::generateModel, py::arg("shapeAttributes"),
	             py::arg("rulePackagePath"), py::arg("geometryEncoderName"), py::arg("geometryEncoderOptions"),
	             doc::MgGen)
	        .def("generate_model", &ModelGenerator::generateAnotherModel, py::arg("shapeAttributes"), doc::MgGen2);

	py::class_<GeneratedModel>(m, "GeneratedModel", doc::Gm)
	        .def("get_initial_shape_index", &GeneratedModel::getInitialShapeIndex, doc::GmGetInd)
	        .def("get_vertices", &GeneratedModel::getVertices, doc::GmGetV)
	        .def("get_indices", &GeneratedModel::getIndices, doc::GmGetI)
	        .def("get_faces", &GeneratedModel::getFaces, doc::GmGetF)
	        .def("get_report", &GeneratedModel::getReport, doc::GmGetR);

	py::class_<std::filesystem::path>(m, "Path").def(py::init<std::string>());
	py::implicitly_convertible<std::string, std::filesystem::path>();

} // PYBIND11_MODULE
