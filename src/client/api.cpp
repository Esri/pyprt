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

#include "InitialShape.h"
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
constexpr const wchar_t* NO_KEY = L"#NULL#";

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

void getAnnotations(const prt::RuleFileInfo::Entry* attribute, std::vector<std::vector<py::object>>& annotations,
                    bool& hidden) {
	for (size_t f = 0; f < attribute->getNumAnnotations(); f++) {
		const prt::Annotation* annot = attribute->getAnnotation(f);
		if (std::wcscmp(annot->getName(), ANNOT_HIDDEN) == 0) {
			hidden = true;
			break;
		}

		if (std::wcsncmp(annot->getName(), L"@", 1) == 0) {
			std::vector<py::object> annotationPyList;
			annotationPyList.push_back(py::cast(annot->getName()));

			for (size_t u = 0; u < annot->getNumArguments(); u++) {
				py::object annotationValue;
				const prt::AnnotationArgument* annotationArg = annot->getArgument(u);
				const prt::AnnotationArgumentType annotationValueType = annotationArg->getType();

				if (annotationValueType == prt::AAT_STR)
					annotationValue = py::cast(annotationArg->getStr());
				else if (annotationValueType == prt::AAT_BOOL)
					annotationValue = py::cast(annotationArg->getBool());
				else if (annotationValueType == prt::AAT_FLOAT)
					annotationValue = py::cast(annotationArg->getFloat());
				else
					annotationValue = py::cast("UNKNOWN_PARAMETER_VALUE_TYPE");

				py::list annotationParameters;
				if (std::wcscmp(annotationArg->getKey(), NO_KEY) == 0)
					annotationParameters.append(py::cast(NO_KEY));
				else
					annotationParameters.append(py::cast(annotationArg->getKey()));
				annotationParameters.append(annotationValue);
				annotationPyList.push_back(annotationParameters);
			}

			annotations.push_back(annotationPyList);
		}
	}
}

py::str getAnnotationArgumentTypeString(const prt::AnnotationArgumentType& valueType) {
	py::str type;
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
	return type;
}

py::dict getRuleAttributes(const prt::RuleFileInfo* ruleFileInfo) {
	auto ruleAttrs = py::dict();

	for (size_t i = 0; i < ruleFileInfo->getNumAttributes(); i++) {
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo->getAttribute(i);
		bool hidden = false;
		std::vector<std::vector<py::object>> annotations;
		const std::wstring fullName(attr->getName());
		if (fullName.find(L"Default$") != 0)
			continue;
		if (attr->getNumParameters() > 0)
			continue;
		const std::wstring name = fullName.substr(8);
		getAnnotations(attr, annotations, hidden);

		const prt::AnnotationArgumentType valueType = attr->getReturnType();
		auto dictAttr = py::dict();
		if (!hidden) {
			dictAttr[py::cast("type")] = getAnnotationArgumentTypeString(valueType);
			dictAttr[py::cast("annotations")] = annotations;
			ruleAttrs[py::cast(name)] = dictAttr;
		}
	}

	return ruleAttrs;
}

py::dict getRPKInfo(const std::filesystem::path& rulePackagePath) {
	ResolveMapPtr resolveMap;

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
	RuleFileInfoUPtr info(prt::createRuleFileInfo(ruleFileURI, nullptr, &infoStatus));
	if (!info || infoStatus != prt::STATUS_OK) {
		LOG_ERR << "could not get rule file info from rule file " << ruleFile;
		return py::dict();
	}

	py::dict ruleAttrs = getRuleAttributes(info.get());

	return ruleAttrs;
}

py::dict getRuleAttributesDeprecated(const prt::RuleFileInfo* ruleFileInfo) {
	auto ruleAttrs = py::dict();

	for (size_t i = 0; i < ruleFileInfo->getNumAttributes(); i++) {
		bool hidden = false;
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo->getAttribute(i);

		const std::wstring fullName(attr->getName());
		if (fullName.find(L"Default$") != 0)
			continue;
		const std::wstring name = fullName.substr(8);
		const prt::AnnotationArgumentType valueType = attr->getReturnType();

		for (size_t f = 0; f < attr->getNumAnnotations(); f++) {
			if (!(std::wcscmp(attr->getAnnotation(f)->getName(), ANNOT_HIDDEN))) {
				hidden = true;
				break;
			}
		}

		if (!hidden) {
			ruleAttrs[py::cast(name)] = getAnnotationArgumentTypeString(valueType);;
		}
	}

	return ruleAttrs;
}

py::dict inspectRPKDeprecated(const std::filesystem::path& rulePackagePath) {
	PyErr_WarnEx(PyExc_DeprecationWarning, "inspect_rpk(rule_package_path) is deprecated, use get_rpk_attributes_info(rule_package_path) instead.", 1);
	ResolveMapPtr resolveMap;

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
	RuleFileInfoUPtr info(prt::createRuleFileInfo(ruleFileURI, nullptr, &infoStatus));
	if (!info || infoStatus != prt::STATUS_OK) {
		LOG_ERR << "could not get rule file info from rule file " << ruleFile;
		return py::dict();
	}

	py::dict ruleAttrs = getRuleAttributesDeprecated(info.get());

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
	m.def("inspect_rpk", &inspectRPKDeprecated, py::arg("rulePackagePath"), doc::InspectRPKDeprecated);
	m.def("get_rpk_attributes_info", &getRPKInfo, py::arg("rulePackagePath"), doc::GetRPKInfo);
	m.attr("NO_KEY") = NO_KEY;

	py::class_<InitialShape>(m, "InitialShape", doc::Is)
	        .def(py::init<const Coordinates&>(), py::arg("vertCoordinates"), doc::IsInitV)
	        .def(py::init<const Coordinates&, const Indices&, const Indices&, const HoleIndices&>(),
	             py::arg("vertCoordinates"), py::arg("faceVertIndices"), py::arg("faceVertCount"),
	             py::arg("holes") = HoleIndices(), doc::IsInitVI)
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
	        .def("generate_model", &ModelGenerator::generateAnotherModel, py::arg("shapeAttributes"));

	py::class_<GeneratedModel>(m, "GeneratedModel", doc::Gm)
	        .def("get_initial_shape_index", &GeneratedModel::getInitialShapeIndex, doc::GmGetInd)
	        .def("get_vertices", &GeneratedModel::getVertices, doc::GmGetV)
	        .def("get_indices", &GeneratedModel::getIndices, doc::GmGetI)
	        .def("get_faces", &GeneratedModel::getFaces, doc::GmGetF)
	        .def("get_report", &GeneratedModel::getReport, doc::GmGetR)
	        .def("get_cga_prints", &GeneratedModel::getCGAPrints, doc::GmGetP)
	        .def("get_cga_errors", &GeneratedModel::getCGAErrors, doc::GmGetE)
			.def("get_attributes_values", &GeneratedModel::getAttributesValues);

	py::class_<std::filesystem::path>(m, "Path").def(py::init<std::string>());
	py::implicitly_convertible<std::string, std::filesystem::path>();

} // PYBIND11_MODULE
