/**
 * ArcGIS CityEngine SDK Geometry Encoder for Python
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
 */

#define _CRT_SECURE_NO_WARNINGS

#include "PyCallbacks.h"
#include "logging.h"
#include "utils.h"
#include "wrap.h"

#include "prt/API.h"
#include "prt/ContentType.h"
#include "prt/LogLevel.h"
#include "prt/prt.h"

#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#ifdef _WIN32
#	include <direct.h>
#endif

/**
 * commonly used constants
 */
const wchar_t* FILE_CGA_REPORT = L"CGAReport.txt";
const wchar_t* ENCODER_OPT_NAME = L"name";

const std::wstring ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const std::wstring ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
const std::wstring ENCODER_ID_PYTHON = L"com.esri.pyprt.PyEncoder";

PYBIND11_MAKE_OPAQUE(std::vector<GeneratedModel>);

namespace {

std::unique_ptr<PRTContext> prtCtx;

void initializePRT() {
	if (!prtCtx)
		prtCtx.reset(new PRTContext(prt::LOG_ERROR));
}

bool isPRTInitialized() {
	return (bool)prtCtx;
}

void shutdownPRT() {
	prtCtx.reset();
}

} // namespace

namespace py = pybind11;

InitialShape::InitialShape(const std::vector<double>& vert) : mVertices(vert), mPathFlag(false) {
	mIndices.resize(vert.size() / 3);
	std::iota(std::begin(mIndices), std::end(mIndices), 0);
	mFaceCounts.resize(1, (uint32_t)mIndices.size());
}

InitialShape::InitialShape(const std::vector<double>& vert, const std::vector<uint32_t>& ind,
                           const std::vector<uint32_t>& faceCnt)
    : mVertices(vert), mIndices(ind), mFaceCounts(faceCnt), mPathFlag(false) {}

InitialShape::InitialShape(const std::string& initShapePath) : mPath(initShapePath), mPathFlag(true) {}

GeneratedModel::GeneratedModel(const size_t& initShapeIdx, const std::vector<double>& vert,
                               const std::vector<uint32_t>& indices, const std::vector<uint32_t>& face,
                               const py::dict& rep)
    : mInitialShapeIndex(initShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mReport(rep) {}

namespace {

void extractMainShapeAttributes(const py::dict& shapeAttr, std::wstring& ruleFile, std::wstring& startRule,
                                int32_t& seed, std::wstring& shapeName, pcu::AttributeMapPtr& convertShapeAttr) {
	convertShapeAttr = pcu::createAttributeMapFromPythonDict(
	        shapeAttr, *(pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));
	if (convertShapeAttr) {
		if (convertShapeAttr->hasKey(L"ruleFile") &&
		    convertShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
			ruleFile = convertShapeAttr->getString(L"ruleFile");
		if (convertShapeAttr->hasKey(L"startRule") &&
		    convertShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
			startRule = convertShapeAttr->getString(L"startRule");
		if (convertShapeAttr->hasKey(L"seed") && convertShapeAttr->getType(L"seed") == prt::AttributeMap::PT_INT)
			seed = convertShapeAttr->getInt(L"seed");
		if (convertShapeAttr->hasKey(L"shapeName") &&
		    convertShapeAttr->getType(L"shapeName") == prt::AttributeMap::PT_STRING)
			shapeName = convertShapeAttr->getString(L"shapeName");
	}
}

ModelGenerator::ModelGenerator(const std::vector<InitialShape>& myGeo) {
	mInitialShapesBuilders.resize(myGeo.size());

	mCache = (pcu::CachePtr)prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

	// Initial shapes initializing
	for (size_t ind = 0; ind < myGeo.size(); ind++) {

		pcu::InitialShapeBuilderPtr isb{prt::InitialShapeBuilder::create()};

		if (myGeo[ind].getPathFlag()) {
			if (!pcu::toFileURI(myGeo[ind].getPath()).empty()) {
				LOG_DBG << "trying to read initial shape geometry from " << pcu::toFileURI(myGeo[ind].getPath())
				        << std::endl;
				const prt::Status s =
				        isb->resolveGeometry(pcu::toUTF16FromOSNarrow(pcu::toFileURI(myGeo[ind].getPath())).c_str(),
				                             mResolveMap.get(), mCache.get());
				if (s != prt::STATUS_OK) {
					LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(myGeo[ind].getPath());
					mValid = false;
				}
			}
			else {
				LOG_ERR << "could not read initial shape geometry, unvalid path";
				mValid = false;
			}
		}
		else {
			if (isb->setGeometry(myGeo[ind].getVertices(), myGeo[ind].getVertexCount(), myGeo[ind].getIndices(),
			                     myGeo[ind].getIndexCount(), myGeo[ind].getFaceCounts(),
			                     myGeo[ind].getFaceCountsCount()) != prt::STATUS_OK) {

				LOG_ERR << "invalid initial geometry";
				mValid = false;
			}
		}

		if (mValid)
			mInitialShapesBuilders[ind] = std::move(isb);
	}
}

void ModelGenerator::setAndCreateInitialShape(const std::vector<py::dict>& shapesAttr,
                                              std::vector<const prt::InitialShape*>& initShapes,
                                              std::vector<pcu::InitialShapePtr>& initShapePtrs,
                                              std::vector<pcu::AttributeMapPtr>& convertedShapeAttr) {
	for (size_t ind = 0; ind < mInitialShapesBuilders.size(); ind++) {
		py::dict shapeAttr = shapesAttr[0];
		if (shapesAttr.size() > ind)
			shapeAttr = shapesAttr[ind];

		std::wstring ruleF = mRuleFile;
		std::wstring startR = mStartRule;
		int32_t randomS = mSeed;
		std::wstring shapeN = mShapeName;
		extractMainShapeAttributes(shapeAttr, ruleF, startR, randomS, shapeN, convertedShapeAttr[ind]);

		mInitialShapesBuilders[ind]->setAttributes(ruleF.c_str(), startR.c_str(), randomS, shapeN.c_str(),
		                                           convertedShapeAttr[ind].get(), mResolveMap.get());

		initShapePtrs[ind].reset(mInitialShapesBuilders[ind]->createInitialShape());
		initShapes[ind] = initShapePtrs[ind].get();
	}
}

void ModelGenerator::initializeEncoderData(const std::wstring& encName, const py::dict& encOpt) {
	mEncodersNames.clear();
	mEncodersOptionsPtr.clear();

	mEncodersNames.push_back(encName);
	const pcu::AttributeMapPtr encOptions{pcu::createAttributeMapFromPythonDict(encOpt, *mEncoderBuilder)};
	mEncodersOptionsPtr.push_back(createValidatedOptions(encName.c_str(), encOptions));

	if (encName != ENCODER_ID_PYTHON) {
		mEncodersNames.push_back(ENCODER_ID_CGA_REPORT);
		mEncodersNames.push_back(ENCODER_ID_CGA_PRINT);

		const pcu::AttributeMapBuilderPtr optionsBuilder{prt::AttributeMapBuilder::create()};
		optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
		const pcu::AttributeMapPtr reportOptions{optionsBuilder->createAttributeMapAndReset()};
		const pcu::AttributeMapPtr printOptions{optionsBuilder->createAttributeMapAndReset()};

		mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions));
		mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions));
	}
}

void ModelGenerator::getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc,
                                               std::vector<const prt::AttributeMap*>& allEncOpt) {
	if (mEncodersNames[0] == ENCODER_ID_PYTHON) {
		allEnc.clear();
		allEncOpt.clear();

		allEnc.push_back(mEncodersNames[0].c_str());
		allEncOpt.push_back(mEncodersOptionsPtr[0].get());
	}
	else {
		allEnc.clear();
		allEnc.push_back(mEncodersNames[0].c_str());
		allEnc.push_back(mEncodersNames[1].c_str()); // an encoder to redirect CGA report to CGAReport.txt
		allEnc.push_back(mEncodersNames[2].c_str()); // redirects CGA print output to the callback

		allEncOpt.clear();
		allEncOpt.push_back(mEncodersOptionsPtr[0].get());
		allEncOpt.push_back(mEncodersOptionsPtr[1].get());
		allEncOpt.push_back(mEncodersOptionsPtr[2].get());
	}
}

std::vector<GeneratedModel> ModelGenerator::generateModel(const std::vector<py::dict>& shapeAttributes,
                                                          const std::string& rulePackagePath,
                                                          const std::wstring& geometryEncoderName,
                                                          const py::dict& geometryEncoderOptions) {
	if (!mValid) {
		LOG_ERR << "invalid ModelGenerator instance.";
		return {};
	}

	if ((shapeAttributes.size() != 1) &&
	    (shapeAttributes.size() <
	     mInitialShapesBuilders.size())) { // if one shape attribute dictionary, same apply to all initial shapes.
		LOG_ERR << "not enough shape attributes dictionaries defined.";
		return {};
	}
	else if (shapeAttributes.size() > mInitialShapesBuilders.size()) {
		LOG_WRN << "number of shape attributes dictionaries defined greater than number of initial shapes given."
		        << std::endl;
	}

	std::vector<GeneratedModel> newGeneratedGeo;
	newGeneratedGeo.reserve(mInitialShapesBuilders.size());

	try {
		if (!prtCtx) {
			LOG_ERR << "prt has not been initialized.";
			return {};
		}

		// Resolve Map
		if (!rulePackagePath.empty()) {
			LOG_INF << "using rule package " << rulePackagePath << std::endl;

			const std::string u8rpkURI = pcu::toFileURI(rulePackagePath);
			prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
			try {
				mResolveMap.reset(prt::createResolveMap(pcu::toUTF16FromUTF8(u8rpkURI).c_str(), nullptr, &status));
			}
			catch (std::exception& e) {
				pybind11::print("CAUGHT EXCEPTION:", e.what());
			}

			if (mResolveMap && (status == prt::STATUS_OK)) {
				LOG_DBG << "resolve map = " << pcu::objectToXML(mResolveMap.get()) << std::endl;
			}
			else {
				LOG_ERR << "getting resolve map from '" << rulePackagePath << "' failed, aborting.";
				return {};
			}
		}

		// Initial shapes
		std::vector<const prt::InitialShape*> initialShapes(mInitialShapesBuilders.size());
		std::vector<pcu::InitialShapePtr> initialShapePtrs(mInitialShapesBuilders.size());
		std::vector<pcu::AttributeMapPtr> convertedShapeAttrVec(mInitialShapesBuilders.size());
		setAndCreateInitialShape(shapeAttributes, initialShapes, initialShapePtrs, convertedShapeAttrVec);

		// Encoder info, encoder options
		if (!mEncoderBuilder)
			mEncoderBuilder.reset(prt::AttributeMapBuilder::create());

		if (!geometryEncoderName.empty())
			initializeEncoderData(geometryEncoderName, geometryEncoderOptions);

		std::vector<const wchar_t*> encoders;
		encoders.reserve(3);
		std::vector<const prt::AttributeMap*> encodersOptions;
		encodersOptions.reserve(3);

		getRawEncoderDataPointers(encoders, encodersOptions);

		if (mEncodersNames[0] == ENCODER_ID_PYTHON) {

			pcu::PyCallbacksPtr foc{std::make_unique<PyCallbacks>(mInitialShapesBuilders.size())};

			// Generate
			const prt::Status genStat =
			        prt::generate(initialShapes.data(), initialShapes.size(), nullptr, encoders.data(), encoders.size(),
			                      encodersOptions.data(), foc.get(), mCache.get(), nullptr);

			if (genStat != prt::STATUS_OK) {
				LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' ("
				        << genStat << ")";
				return {};
			}

			for (size_t idx = 0; idx < mInitialShapesBuilders.size(); idx++) {
				newGeneratedGeo.emplace_back(idx, foc->getVertices(idx), foc->getIndices(idx), foc->getFaces(idx),
				                             foc->getReport(idx));
			}
		}
		else {
			const std::filesystem::path outputPath = geometryEncoderOptions["outputPath"].cast<std::string>();
			LOG_DBG << "got outputPath = " << outputPath;

			pcu::FileOutputCallbacksPtr foc;
			if (std::filesystem::is_directory(outputPath) && std::filesystem::exists(outputPath)) {
				foc.reset(prt::FileOutputCallbacks::create(outputPath.wstring().c_str()));
			}
			else {
				LOG_ERR << "The directory specified by 'outputPath' is not valid or does not exist: " << outputPath
				        << std::endl;
				return {};
			}

			// Generate
			const prt::Status genStat =
			        prt::generate(initialShapes.data(), initialShapes.size(), nullptr, encoders.data(), encoders.size(),
			                      encodersOptions.data(), foc.get(), mCache.get(), nullptr);

			if (genStat != prt::STATUS_OK) {
				LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' ("
				        << genStat << ")";
				return {};
			}

			return {};
		}
	}
	catch (const std::exception& e) {
		LOG_ERR << "caught exception: " << e.what();
		return {};
	}
	catch (...) {
		LOG_ERR << "caught unknown exception.";
		return {};
	}

	return newGeneratedGeo;
}

std::vector<GeneratedModel> ModelGenerator::generateAnotherModel(const std::vector<py::dict>& shapeAttributes) {
	if (!mResolveMap) {
		LOG_ERR << "generate model with all required parameters";
		return {};
	}
	else
		return generateModel(shapeAttributes, "", L"", {});
}

} // namespace

using namespace pybind11::literals;

PYBIND11_MODULE(pyprt, m) {
	py::options options;
	options.disable_function_signatures();

	py::bind_vector<std::vector<GeneratedModel>>(m, "GeneratedModelVector", py::module_local(false));

	m.def("initialize_prt", &initializePRT, "Initialization of PRT.");
	m.def("is_prt_initialized", &isPRTInitialized, R"mydelimiter(
        is_prt_initialized() -> bool

        This function returns *True* if PRT is initialized, *False* otherwise.

        :Returns:
            bool
    )mydelimiter");
	m.def("shutdown_prt", &shutdownPRT, "Shutdown of PRT. This function can be called only once per session/script.");

	py::class_<InitialShape>(m, "InitialShape", R"mydelimiter(
        __init__(*args, **kwargs)

        The initial shape corresponds to the geometry on which the CGA rule will be applied.
        )mydelimiter")
	                .def(py::init<const std::vector<double>&>(), py::arg("vertCoordinates"), R"mydelimiter(
        **__init__** (*vert_coordinates*)

        InitialShape constructor using a vector of geometry vertices coordinates. The geometry contains 
        one face with no hole.

        :Parameters:
            **vert_coordinates** -- List[float]
        )mydelimiter")
	                .def(py::init<const std::vector<double>&, const std::vector<uint32_t>&,
	                              const std::vector<uint32_t>&>(),
	                     py::arg("vertCoordinates"), py::arg("faceVertIndices"), py::arg("faceVertCount"),
	                     R"mydelimiter(
        **__init__** (*vert_coordinates*, *face_indices*, *face_count*)

        InitialShape constructor using a vector of geometry vertices coordinates, the vertices indices for 
        each face and the indices count per face.

        :Parameters:
            - **vert_coordinates** -- List[float]
            - **face_indices** -- List[int]
            - **face_count** -- List[int]
        )mydelimiter")
	                .def(py::init<const std::string&>(), py::arg("initialShapePath"), R"mydelimiter(
        **__init__** (*init_shape_path*)

        InitialShape constructor using the path to the shape file. This can be an OBJ file, Collada, etc.

        :Parameters:
            **initialShapePath** -- str
        )mydelimiter")
	                .def("get_vertex_count", &InitialShape::getVertexCount, R"mydelimiter(
        get_vertex_count() -> int
        Returns the number of vertices coordinates the initial shape contains.

        :Returns:
            int
        )mydelimiter")
	                .def("get_index_count", &InitialShape::getIndexCount, R"mydelimiter(
        get_index_count() -> int
        Returns the length of the vector containing the vertices indices of the shape.

        :Returns:
            int
        )mydelimiter")
	                .def("get_face_counts_count", &InitialShape::getFaceCountsCount, R"mydelimiter(
        get_face_counts_count() -> int
        Returns the number of faces the initial shape contains.

        :Returns:
            int
        )mydelimiter")
	                .def("get_path", &InitialShape::getPath, R"mydelimiter(
        get_path() -> str
        Returns the initial shape file path.

        :Returns:
            str
        )mydelimiter");

	py::class_<ModelGenerator>(m, "ModelGenerator",
	                           "The ModelGenerator class will host the data required to procedurally generate the 3D "
	                           "model on a given initial shape.")
	        .def(py::init<const std::vector<InitialShape>&>(), py::arg("initialShapes"), R"mydelimiter(
        __init__(init_shapes)

        ModelGenerator constructor using a list of InitialShape instances.

        :Parameters:
            **init_shapes** -- List[InitialShape]

        )mydelimiter")
	        .def("generate_model", &ModelGenerator::generateModel, py::arg("shapeAttributes"),
	             py::arg("rulePackagePath"), py::arg("geometryEncoderName"),
	             py::arg("geometryEncoderOptions"), R"mydelimiter(
        generate_model(*args, **kwargs) -> List[GeneratedModel]

        This function does the procedural generation of the models. It outputs a list of GeneratedModel instances.

        :Parameters:
            - **shape_attributes** -- List[dict]
            - **rule_package_path** -- str
            - **geometry_encoder** -- str
            - **encoder_options** -- dict

        :Returns:
            List[GeneratedModel]
        )mydelimiter")
	        .def("generate_model", &ModelGenerator::generateAnotherModel, py::arg("shapeAttributes"), R"mydelimiter(
        This function can only be used once the previous ModelGenerator::generate_model method has been 
        called. It is useful to specify different shape attribues but use the same CGA rule package on the
        same initial shapes, the same encoder and encoder options.

        :Parameters:
            **shape_attributes** -- List[dict]

        :Returns:
            List[GeneratedModel]

        )mydelimiter");

	py::class_<GeneratedModel>(
	        m, "GeneratedModel",
	        "The GeneratedModel instance contains the generated 3D geometry. This class is only employed if the "
	        "*com.esri.pyprt.PyEncoder* encoder is used in the ModelGenerator instance.")
	        .def("get_initial_shape_index", &GeneratedModel::getInitialShapeIndex, R"mydelimiter(
        get_initial_shape_index() -> int
        Returns the index of the initial shape on which the generated geometry has been built.

        :Returns:
            int
        )mydelimiter")
	        .def("get_vertices", &GeneratedModel::getVertices, R"mydelimiter(
        get_vertices() -> List[float]

        Returns the generated 3D geometry vertices coordinates. If the *emitGeometry* entry of the encoder 
        options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[float]
        )mydelimiter")
	        .def("get_indices", &GeneratedModel::getIndices, R"mydelimiter(
        get_indices() -> List[int]
        
        Returns the generated 3D geometry vertices indices. If the *emitGeometry* entry of the encoder
        options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[int]
        )mydelimiter")
	        .def("get_faces", &GeneratedModel::getFaces, R"mydelimiter(
        get_faces() -> List[int]
        Returns the generated 3D geometry vertices indices per face. If the *emitGeometry* entry of the
        encoder options dictionary has been set to *False*, this function returns an empty vector.

        :Returns:
            List[int]
        )mydelimiter")
	        .def("get_report", &GeneratedModel::getReport, R"mydelimiter(
        get_report() -> dict
        Returns the generated 3D geometry CGA report. This report dictionary can be empty if the CGA rule
        file employed does not output any report or if the *emitReport* entry of the encoder options
        dictionary has been set to *False*.

        :Returns:
            dict
        )mydelimiter");
}
