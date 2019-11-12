/**
 * Esri CityEngine SDK Geometry Encoder for Python
 *
 * Copyright 2014-2019 Esri R&D Zurich and VRBN
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

#include "wrap.h"
#include "utils.h"
#include "logging.h"
#include "PyCallbacks.h"

#include "prt/prt.h"
#include "prt/API.h"
#include "prt/ContentType.h"
#include "prt/LogLevel.h"

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/stl_bind.h>

#include <string>
#include <vector>
#include <iterator>
#include <functional>
#include <array>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <numeric>
#include <filesystem>
#ifdef _WIN32
#	include <direct.h>
#endif

#include <ctime>

/**
  * commonly used constants
  */
const wchar_t* FILE_CGA_REPORT          = L"CGAReport.txt";
const wchar_t* ENCODER_OPT_NAME         = L"name";

const std::wstring ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const std::wstring ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
const std::wstring ENCODER_ID_PYTHON = L"com.esri.prt.examples.PyEncoder";

PYBIND11_MAKE_OPAQUE(std::vector<GeneratedGeometry>);

namespace {

    std::unique_ptr<PRTContext> prtCtx;

    void initializePRT() {
	    if (!prtCtx) prtCtx.reset(new PRTContext(prt::LOG_DEBUG));
    }

    bool isPRTInitialized() {
	    return (bool)prtCtx;
    }

    void shutdownPRT() {
	    prtCtx.reset();
    }

} // namespace

namespace py = pybind11;

Geometry::Geometry(const std::vector<double>& vert) :
    mVertices(vert)
 {
    mIndices.resize(vert.size() / 3);
    std::iota(std::begin(mIndices), std::end(mIndices), 0);
    mFaceCounts.resize(1, (uint32_t)mIndices.size());
}

Geometry::Geometry(const std::vector<double>& vert, const std::vector<uint32_t>& ind, const std::vector<uint32_t>& faceCnt) :
    mVertices(vert), mIndices(ind), mFaceCounts(faceCnt)
{
}


GeneratedGeometry::GeneratedGeometry(const size_t& initShapeIdx, const std::vector<std::vector<double>>& vert, const std::vector<std::vector<uint32_t>>& face, const py::dict& rep) :
    mInitialShapeIndex(initShapeIdx), mVertices(vert), mFaces(face), mReport(rep)
{
}


namespace {

    void extractMainShapeAttributes(const py::dict& shapeAttr, std::wstring& ruleFile, std::wstring& startRule, pcu::AttributeMapPtr& convertShapeAttr) {
        convertShapeAttr = pcu::createAttributeMapFromPythonDict(shapeAttr, *(pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));
        if (convertShapeAttr) {
            if (convertShapeAttr->hasKey(L"ruleFile") &&
                convertShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
                ruleFile = convertShapeAttr->getString(L"ruleFile");
            if (convertShapeAttr->hasKey(L"startRule") &&
                convertShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
                startRule = convertShapeAttr->getString(L"startRule");
        }
    }

    ModelGenerator::ModelGenerator(const std::string& initShapePath) {
        mInitialShapesBuilders.resize(1);

        mCache = (pcu::CachePtr) prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

        // Initial shape initializing
        pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

        if (!pcu::toFileURI(initShapePath).empty()) {
            LOG_DBG << "trying to read initial shape geometry from " << pcu::toFileURI(initShapePath) << std::endl;
            const prt::Status s = isb->resolveGeometry(pcu::toUTF16FromOSNarrow(pcu::toFileURI(initShapePath)).c_str(), mResolveMap.get(), mCache.get());
            if (s != prt::STATUS_OK) {
                LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(initShapePath);
                mValid = false;
            }
        }
        else {
            LOG_ERR << "could not read initial shape geometry, unvalid path";
            mValid = false;
        }

        if (mValid)
            mInitialShapesBuilders[0] = std::move(isb);
    }

    ModelGenerator::ModelGenerator(const std::vector<Geometry>& myGeo) {
        mInitialShapesBuilders.resize(myGeo.size());

        mCache = (pcu::CachePtr) prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

        // Initial shapes initializing
        for (size_t ind = 0; ind < myGeo.size(); ind++) {

            pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

            if (isb->setGeometry(
                myGeo[ind].getVertices(), myGeo[ind].getVertexCount(),
                myGeo[ind].getIndices(), myGeo[ind].getIndexCount(),
                myGeo[ind].getFaceCounts(), myGeo[ind].getFaceCountsCount()) != prt::STATUS_OK) {

                LOG_ERR << "invalid initial geometry";
                mValid = false;
            }

            if (mValid)
                mInitialShapesBuilders[ind] = std::move(isb);
        }
    }

    void ModelGenerator::setAndCreateInitialShape(const pcu::AttributeMapPtr& shapeAttr, std::vector<const prt::InitialShape*>& initShapes, std::vector<pcu::InitialShapePtr>& initShapePtrs)
    {
        for (size_t ind = 0; ind < mInitialShapesBuilders.size(); ind++) {

            mInitialShapesBuilders[ind]->setAttributes(
                mRuleFile.c_str(),
                mStartRule.c_str(),
                mSeed,
                mShapeName.c_str(),
                shapeAttr.get(),
                mResolveMap.get()
            );

            initShapePtrs[ind].reset(mInitialShapesBuilders[ind]->createInitialShape());
            initShapes[ind] = initShapePtrs[ind].get();
        }
    }

    void ModelGenerator::initializeEncoderData(const std::wstring& encName, const py::dict& encOpt) {
        mEncodersNames.clear();
        mEncodersOptionsPtr.clear();

        mEncodersNames.push_back(encName);
        const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapFromPythonDict(encOpt, *mEncoderBuilder) };
        mEncodersOptionsPtr.push_back(createValidatedOptions(encName.c_str(), encOptions));

        if (encName != ENCODER_ID_PYTHON) {
            mEncodersNames.push_back(ENCODER_ID_CGA_REPORT);
            mEncodersNames.push_back(ENCODER_ID_CGA_PRINT);

            const pcu::AttributeMapBuilderPtr optionsBuilder{ prt::AttributeMapBuilder::create() };
            optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
            const pcu::AttributeMapPtr reportOptions{ optionsBuilder->createAttributeMapAndReset() };
            const pcu::AttributeMapPtr printOptions{ optionsBuilder->createAttributeMapAndReset() };

            mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions));
            mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions));
        }

    }

    void ModelGenerator::getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc, std::vector<const prt::AttributeMap*>& allEncOpt) {
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

    std::vector<GeneratedGeometry> ModelGenerator::generateModel(const py::dict& shapeAttributes,
            const std::string& rulePackagePath,
            const std::wstring& geometryEncoderName,
            const py::dict& geometryEncoderOptions)
    {
        if (!mValid) {
            LOG_ERR << "invalid ModelGenerator instance";
            return {};
        }

        std::vector<GeneratedGeometry> newGeneratedGeo;
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

            // Initial shape attributes
            pcu::AttributeMapPtr convertedShapeAttr;
            extractMainShapeAttributes(shapeAttributes, mRuleFile, mStartRule, convertedShapeAttr);

            // Initial shapes
            std::vector<const prt::InitialShape*> initialShapes(mInitialShapesBuilders.size());
            std::vector<pcu::InitialShapePtr> initialShapePtrs(mInitialShapesBuilders.size());
            setAndCreateInitialShape(convertedShapeAttr, initialShapes, initialShapePtrs);

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

                pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>(mInitialShapesBuilders.size()) };

                // Generate
                const prt::Status genStat = prt::generate(
                    initialShapes.data(), initialShapes.size(), nullptr,
                    encoders.data(), encoders.size(), encodersOptions.data(),
                    foc.get(), mCache.get(), nullptr
                );

                if (genStat != prt::STATUS_OK) {
                    LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                    return {};
                }

                for (size_t idx = 0; idx < mInitialShapesBuilders.size(); idx++) {
                    newGeneratedGeo.emplace_back(idx, convertVerticesIntoPythonStyle(foc->getVertices(idx)), foc->getFaces(idx), foc->getReport(idx));
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
                    LOG_ERR << "The directory specified by 'outputPath' is not valid or does not exist: " << outputPath << std::endl;
                    return {};
                }

                // Generate
                const prt::Status genStat = prt::generate(
                    initialShapes.data(), initialShapes.size(), nullptr,
                    encoders.data(), encoders.size(), encodersOptions.data(),
                    foc.get(), mCache.get(), nullptr
                );

                if (genStat != prt::STATUS_OK) {
                    LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
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

    
    std::vector<GeneratedGeometry> ModelGenerator::generateAnotherModel(const py::dict& shapeAttributes)
    {
        if (!mResolveMap) {
            LOG_ERR << "generate model with all required parameters";
            return {};
        }
        else
            return generateModel(shapeAttributes, "", L"", {});
    }

} // namespace


int py_printVal(int val) {
    return val;
}

using namespace pybind11::literals;

PYBIND11_MODULE(pyprt, m) {
    py::bind_vector<std::vector<GeneratedGeometry>>(m, "GeneratedGeometryVector", py::module_local(false));

    py::class_<ModelGenerator>(m, "ModelGenerator")
        .def(py::init<const std::string&>(), "initShapePath"_a)
        .def(py::init<const std::vector<Geometry>&>(), "initShape"_a)
        .def("generate_model", &ModelGenerator::generateModel, py::arg("shapeAttributes"), py::arg("rulePackagePath"), py::arg("geometryEncoderName"), py::arg("geometryEncoderOptions"))
        .def("generate_model", &ModelGenerator::generateAnotherModel, py::arg("shapeAttributes"));

    m.def("initialize_prt", &initializePRT);
    m.def("is_prt_initialized", &isPRTInitialized);
    m.def("shutdown_prt", &shutdownPRT);

    py::class_<Geometry>(m, "Geometry")
        .def(py::init<const std::vector<double>&>())
        .def(py::init<const std::vector<double>&, const std::vector<uint32_t>&, const std::vector<uint32_t>&>())
        .def("get_vertices", &Geometry::getVertices)
        .def("get_vertex_count", &Geometry::getVertexCount)
        .def("get_indices", &Geometry::getIndices)
        .def("get_index_count", &Geometry::getIndexCount)
        .def("get_face_counts", &Geometry::getFaceCounts)
        .def("get_face_counts_count", &Geometry::getFaceCountsCount);

    py::class_<GeneratedGeometry>(m, "GeneratedGeometry")
        .def("get_initial_shape_index", &GeneratedGeometry::getInitialShapeIndex)
        .def("get_vertices", &GeneratedGeometry::getVertices)
        .def("get_faces", &GeneratedGeometry::getFaces)
        .def("get_report", &GeneratedGeometry::getReport);

    m.def("print_val", &py_printVal,"Test Python function for value printing.");
}