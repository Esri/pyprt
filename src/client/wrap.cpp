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
const wchar_t* ENCODER_ID_CGA_REPORT    = L"com.esri.prt.core.CGAReportEncoder";
const wchar_t* ENCODER_ID_CGA_PRINT     = L"com.esri.prt.core.CGAPrintEncoder";
const wchar_t* ENCODER_ID_PYTHON        = L"com.esri.prt.examples.PyEncoder";
const wchar_t* ENCODER_OPT_NAME         = L"name";
pcu::Path executablePath;


namespace {

    std::unique_ptr<PRTContext> prtCtx;

    void initializePRT(std::string const & prtPath) {
	    if (!prtCtx) prtCtx.reset(new PRTContext(prt::LOG_DEBUG, prtPath));
    }

    bool isPRTInitialized() {
	    return (bool)prtCtx;
    }

    void shutdownPRT() {
	    prtCtx.reset();
    }

} // namespace

namespace py = pybind11;

Geometry::Geometry(const std::vector<double>& vert) {
    mVertices = vert;
    mVertexCount = vert.size();
    mIndexCount = (size_t)(mVertexCount / 3);
    mIndices.resize(mIndexCount);
    mFaceCountsCount = 1;
    mFaceCounts.resize(1);

    std::iota(std::begin(mIndices), std::end(mIndices), 0);
    mFaceCounts[0] = (uint32_t) mIndexCount;
}

void Geometry::updateGeometry(const std::vector<double>& vert, const size_t& vertCnt, const std::vector<uint32_t>& ind, const size_t& indCnt, const std::vector<uint32_t>& faceCnt, const size_t& faceCntCnt) {
    mVertices = vert;
    mVertexCount = vertCnt;
    mIndices = ind;
    mIndexCount = indCnt;
    mFaceCounts = faceCnt;
    mFaceCountsCount = faceCntCnt;
}


GeneratedGeometry::GeneratedGeometry(const size_t& initShapeIdx, const std::vector<double>& vert, const std::vector<std::vector<uint32_t>>& face, const FloatMap& floatRep, const StringMap& stringRep, const BoolMap& boolRep) {
    mInitialShapeIndex = initShapeIdx;
    mVerticesVect = vert;
    mFaces = face;
    mFloatReport = floatRep;
    mStringReport = stringRep;
    mBoolReport = boolRep;
}

void GeneratedGeometry::convertGeometryIntoPythonStyle() {
    mVertices.resize(mVerticesVect.size() / 3);

    for(size_t i = 0; i < mVerticesVect.size() / 3; i++)
        mVertices[i] = { mVerticesVect[0], mVerticesVect[1], mVerticesVect[2] };
}


namespace {

    ModelGenerator::ModelGenerator(const std::string& initShapePath) {
        initialShapePath = initShapePath;
        initialShapesBuilders.resize(1);

        cache = (pcu::CachePtr) prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

        // Initial shape initializing
        pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

        if (!pcu::toFileURI(initialShapePath).empty()) {
            LOG_DBG << "trying to read initial shape geometry from " << pcu::toFileURI(initialShapePath) << std::endl;
            const prt::Status s = isb->resolveGeometry(pcu::toUTF16FromOSNarrow(pcu::toFileURI(initialShapePath)).c_str(), resolveMap.get(), cache.get());
            if (s != prt::STATUS_OK) {
                LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(initialShapePath);
                valid = false;
            }
        }
        else {
            LOG_ERR << "could not read initial shape geometry, unvalid path";
            valid = false;
        }

        if (valid)
            initialShapesBuilders[0] = std::move(isb);
    }

    ModelGenerator::ModelGenerator(const std::vector<Geometry>& myGeo) {
        initialGeometries = myGeo;
        initialShapesBuilders.resize(myGeo.size());

        cache = (pcu::CachePtr) prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

        // Initial shapes initializing
        for (size_t ind = 0; ind < initialGeometries.size(); ind++) {

            pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

            if (isb->setGeometry(
                initialGeometries[ind].getVertices(), initialGeometries[ind].getVertexCount(),
                initialGeometries[ind].getIndices(), initialGeometries[ind].getIndexCount(),
                initialGeometries[ind].getFaceCounts(), initialGeometries[ind].getFaceCountsCount()) != prt::STATUS_OK) {

                LOG_ERR << "invalid initial geometry";
                valid = false;
            }

            if (valid)
                initialShapesBuilders[ind] = std::move(isb);
        }
    }

    std::vector<GeneratedGeometry> ModelGenerator::generateModel(const std::string& rulePackagePath,
            py::dict shapeAttributes,
            py::dict encoderOptions = {},
            const wchar_t* encoderName = ENCODER_ID_PYTHON)
    {
        if (!valid) {
            LOG_ERR << "invalid ModelGenerator instance";
            return {};
        }

        std::clock_t start;
        double duration;
        start = std::clock();

        std::vector<GeneratedGeometry> newGeneratedGeo;
        newGeneratedGeo.resize(initialShapesBuilders.size());

        try {
            if (!prtCtx) {
                LOG_ERR << "prt has not been initialized.";
                return {};
            }


            // Resolve Map
            if (!resolveMap) {
                if (!rulePackagePath.empty()) {
                    LOG_INF << "using rule package " << rulePackagePath << std::endl;

                    const std::string u8rpkURI = pcu::toFileURI(rulePackagePath);
                    prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
                    try {
                        auto* r = prt::createResolveMap(pcu::toUTF16FromUTF8(u8rpkURI).c_str(), nullptr, &status);
                        resolveMap.reset(r);
                    }
                    catch (std::exception& e) {
                        pybind11::print("CAUGHT EXCEPTION:", e.what());
                    }


                    if (resolveMap && (status == prt::STATUS_OK)) {
                        LOG_DBG << "resolve map = " << pcu::objectToXML(resolveMap.get()) << std::endl;
                    }
                    else {
                        LOG_ERR << "getting resolve map from '" << rulePackagePath << "' failed, aborting.";
                        return {};
                    }
                }
            }


            // Initial shape attributes
            const pcu::AttributeMapPtr convertedShapeAttr{ pcu::createAttributeMapFromPythonDict(shapeAttributes, pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())) };
            if (convertedShapeAttr) {
                if (convertedShapeAttr->hasKey(L"ruleFile") &&
                    convertedShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
                    ruleFile = convertedShapeAttr->getString(L"ruleFile");
                if (convertedShapeAttr->hasKey(L"startRule") &&
                    convertedShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
                    startRule = convertedShapeAttr->getString(L"startRule");
            }


            // Encoder info, encoder options
            const pcu::AttributeMapBuilderPtr optionsBuilder{ prt::AttributeMapBuilder::create() };
            optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
            const pcu::AttributeMapPtr reportOptions{ optionsBuilder->createAttributeMapAndReset() };
            const pcu::AttributeMapPtr printOptions{ optionsBuilder->createAttributeMapAndReset() };

            encoderBuilder = std::move(pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create()));
            const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapFromPythonDict(encoderOptions, encoderBuilder) };

            CGAReportOptions = createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions);
            CGAPrintOptions = createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions);
            pyEncoderOptions = createValidatedOptions(encoderName, encOptions);

            if (!allEncoders.empty()) { 
                delete[] allEncoders[0];
            }

            // Make a copy
            wchar_t* encoder = new wchar_t[wcslen(encoderName) + 1];
            wcsncpy(encoder, encoderName, wcslen(encoderName) + 1);

            allEncoders = {
                    encoder,
                    ENCODER_ID_CGA_REPORT, // an encoder to redirect CGA report to CGAReport.txt
                    ENCODER_ID_CGA_PRINT // redirects CGA print output to the callback
            };

            allEncodersOptions = { pyEncoderOptions.get(), CGAReportOptions.get(), CGAPrintOptions.get() };
            

            // Initial shapes
            std::vector<pcu::InitialShapePtr> initialShapePtrs;
            initialShapePtrs.reserve(initialShapesBuilders.size());
            std::vector<const prt::InitialShape*> initialShapes;
            initialShapes.reserve(initialShapesBuilders.size());

            for (size_t ind = 0; ind < initialShapesBuilders.size(); ind++) {

                initialShapesBuilders[ind]->setAttributes(
                    ruleFile.c_str(),
                    startRule.c_str(),
                    seed,
                    shapeName.c_str(),
                    convertedShapeAttr.get(),
                    resolveMap.get()
                );

                pcu::InitialShapePtr initialShape{ initialShapesBuilders[ind]->createInitialShape() };

                initialShapes.push_back(initialShape.get());
                initialShapePtrs.push_back(std::move(initialShape));
            }

            if (!std::wcscmp(encoderName, ENCODER_ID_PYTHON)) {
                pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>(initialShapesBuilders.size()) };

                // Generate
                const prt::Status genStat = prt::generate(
                    initialShapes.data(), initialShapes.size(), nullptr,
                    allEncoders.data(), allEncoders.size(), allEncodersOptions.data(),
                    foc.get(), cache.get(), nullptr
                );

                if (genStat != prt::STATUS_OK) {
                    LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                    return {};
                }

                for (size_t idx = 0; idx < initialShapesBuilders.size(); idx++) {
                    GeneratedGeometry geo(idx, foc->getVertices(idx), foc->getFaces(idx), foc->getFloatReport(idx), foc->getStringReport(idx), foc->getBoolReport(idx));
                    geo.convertGeometryIntoPythonStyle();
                    newGeneratedGeo[idx] = geo;
                }

            }
            else {
                const pcu::Path output_path = executablePath.getParent().getParent() / "output";
                if (!output_path.exists()) {
                    std::filesystem::create_directory(output_path.toStdPath());
                    LOG_INF << "new output directory created at " << output_path << std::endl;
                }

                pcu::FileOutputCallbacksPtr foc{ prt::FileOutputCallbacks::create(output_path.native_wstring().c_str()) };

                // Generate
                const prt::Status genStat = prt::generate(
                    initialShapes.data(), initialShapes.size(), nullptr,
                    allEncoders.data(), allEncoders.size(), allEncodersOptions.data(),
                    foc.get(), cache.get(), nullptr
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

        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
        std::cout << "Method duration - first model generation: " << duration << std::endl;

        return newGeneratedGeo;
    }

    std::vector<GeneratedGeometry> ModelGenerator::generateAnotherModel(py::dict shapeAttributes, py::dict encoderOptions = {})
    {
        if (!valid) {
            LOG_ERR << "invalid ModelGenerator instance";
            return {};
        }

        std::clock_t start;
        double duration;
        start = std::clock();

        std::vector<GeneratedGeometry> newGeneratedGeo;
        newGeneratedGeo.resize(initialShapesBuilders.size());

        try {
            if (!prtCtx) {
                LOG_ERR << "prt has not been initialized.";
                return {};
            }

            if (!resolveMap) {
                LOG_ERR << "getting resolve map failed, aborting.";
                return {};
            }

            // Initial shape attributes
            const pcu::AttributeMapPtr convertedShapeAttr{ pcu::createAttributeMapFromPythonDict(shapeAttributes, pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())) };
            if (convertedShapeAttr) {
                if (convertedShapeAttr->hasKey(L"ruleFile") &&
                    convertedShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
                    ruleFile = convertedShapeAttr->getString(L"ruleFile");
                if (convertedShapeAttr->hasKey(L"startRule") &&
                    convertedShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
                    startRule = convertedShapeAttr->getString(L"startRule");
            }


            // Encoder info, encoder options
            const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapFromPythonDict(encoderOptions, encoderBuilder) };

            pyEncoderOptions = createValidatedOptions(allEncoders[0], encOptions);
            allEncodersOptions[0] = pyEncoderOptions.get();


            // Initial Shapes
            std::vector<pcu::InitialShapePtr> initialShapePtrs;
            initialShapePtrs.reserve(initialShapesBuilders.size());
            std::vector<const prt::InitialShape*> initialShapes;
            initialShapes.reserve(initialShapesBuilders.size());

            for (size_t ind = 0; ind < initialShapesBuilders.size(); ind++) {

                if (initialShapesBuilders.empty()) {
                    LOG_ERR << "initial shape builders empty.";
                    return {};
                }

                initialShapesBuilders[ind]->setAttributes(
                    ruleFile.c_str(),
                    startRule.c_str(),
                    seed,
                    shapeName.c_str(),
                    convertedShapeAttr.get(),
                    resolveMap.get()
                );

                pcu::InitialShapePtr initialShape{ initialShapesBuilders[ind]->createInitialShape() };

                initialShapes.push_back(initialShape.get());
                initialShapePtrs.push_back(std::move(initialShape));
            }

            if (!std::wcscmp(allEncoders[0], ENCODER_ID_PYTHON)) {

                pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>(initialShapesBuilders.size()) };

                // Generate
                const prt::Status genStat = prt::generate(
                    initialShapes.data(), initialShapes.size(), nullptr,
                    allEncoders.data(), allEncoders.size(), allEncodersOptions.data(),
                    foc.get(), cache.get(), nullptr
                );

                if (genStat != prt::STATUS_OK) {
                    LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                    return {};
                }

                for (size_t idx = 0; idx < initialShapesBuilders.size(); idx++) {
                    GeneratedGeometry geo(idx, foc->getVertices(idx), foc->getFaces(idx), foc->getFloatReport(idx), foc->getStringReport(idx), foc->getBoolReport(idx));
                    geo.convertGeometryIntoPythonStyle();
                    newGeneratedGeo[idx] = geo;
                }

            }
            else {
                const pcu::Path output_path = executablePath.getParent().getParent() / "output";
                if (!output_path.exists()) {
                    std::filesystem::create_directory(output_path.toStdPath());
                    LOG_INF << "new output directory created at " << output_path << std::endl;
                }

                pcu::FileOutputCallbacksPtr foc{ prt::FileOutputCallbacks::create(output_path.native_wstring().c_str()) };

                // Generate
                const prt::Status genStat = prt::generate(
                    initialShapes.data(), initialShapes.size(), nullptr,
                    allEncoders.data(), allEncoders.size(), allEncodersOptions.data(),
                    foc.get(), cache.get(), nullptr
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

        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
        std::cout << "Method duration - second model generation (with other attributes): " << duration << std::endl;

        return newGeneratedGeo;
    }

} // namespace


int py_printVal(int val) {
    return val;
}

using namespace pybind11::literals;

PYBIND11_MODULE(pyprt, m) {
    py::class_<ModelGenerator>(m, "ModelGenerator")
        .def(py::init<const std::string&>(), "initShapePath"_a)
        .def(py::init<const std::vector<Geometry>&>(), "initShape"_a)
        .def("generate_model", &ModelGenerator::generateModel, py::arg("rulePackagePath"), py::arg("shapeAttributes"), py::arg("encoderOptions") = py::dict(), py::arg("encoderName") = ENCODER_ID_PYTHON)
        .def("generate_another_model", &ModelGenerator::generateAnotherModel, py::arg("shapeAttributes"), py::arg("encoderOptions") = py::dict());

    m.def("initialize_prt", &initializePRT, "prt_path"_a = "");
    m.def("is_prt_initialized", &isPRTInitialized);
    m.def("shutdown_prt", &shutdownPRT);

    py::class_<Geometry>(m, "Geometry")
        .def(py::init<>())
        .def(py::init<std::vector<double>>())
        .def("update_geometry",&Geometry::updateGeometry)
        .def("get_vertices", &Geometry::getVertices)
        .def("get_vertex_count", &Geometry::getVertexCount)
        .def("get_indices", &Geometry::getIndices)
        .def("get_index_count", &Geometry::getIndexCount)
        .def("get_face_counts", &Geometry::getFaceCounts)
        .def("get_face_counts_count", &Geometry::getFaceCountsCount);

    py::class_<GeneratedGeometry>(m, "GeneratedGeometry")
        .def(py::init<const size_t&, const std::vector<double>&, const std::vector<std::vector<uint32_t>>&, const FloatMap&, const StringMap&, const BoolMap&>())
        .def("get_initial_shape_index", &GeneratedGeometry::getInitialShapeIndex)
        .def("get_vertices", &GeneratedGeometry::getVertices)
        .def("get_faces", &GeneratedGeometry::getFaces)
        .def("get_float_report", &GeneratedGeometry::getFloatReport)
        .def("get_string_report", &GeneratedGeometry::getStringReport)
        .def("get_bool_report", &GeneratedGeometry::getBoolReport);

    m.def("print_val", &py_printVal,"Test Python function for value printing.");
}