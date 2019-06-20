/**
 * Esri CityEngine SDK - Python Bindings
 * 
 * author: Camille Lechot
 */

#define _CRT_SECURE_NO_WARNINGS

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
#include <direct.h>

/**
    * commonly used constants
    */
const char*    FILE_LOG = "pyprt.log";
const wchar_t* FILE_CGA_REPORT = L"CGAReport.txt";
const wchar_t* ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const wchar_t* ENCODER_ID_PYTHON = L"com.esri.prt.examples.PyEncoder";
const wchar_t* ENCODER_OPT_NAME = L"name";
pcu::Path executablePath;
    

template <typename T>
T* vectorToArray(std::vector<T> data) {
    size_t array_size = data.size();
    T* tmp = new T[array_size];

    for (int i = 0; i < (int)array_size; i++) {
        tmp[i] = data[i];
    }

    return tmp;
}

/**
    * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
    */
struct PRTContext {
    PRTContext(prt::LogLevel defaultLogLevel, std::string const & sdkPath) {
        executablePath = sdkPath.empty() ? pcu::getExecutablePath() : sdkPath;
        const pcu::Path installPath = executablePath.getParent();
        const pcu::Path fsLogPath = installPath / FILE_LOG;

        mLogHandler.reset(prt::ConsoleLogHandler::create(prt::LogHandler::ALL, defaultLogLevel));
        mFileLogHandler.reset(prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT, fsLogPath.native_wstring().c_str()));
        prt::addLogHandler(mLogHandler.get());
        prt::addLogHandler(mFileLogHandler.get());

        // setup paths for plugins, assume standard SDK layout as per README.md
        const pcu::Path extPath = installPath / "lib";

        // initialize PRT with the path to its extension libraries, the default log level
        const std::wstring wExtPath = extPath.native_wstring();
        const std::array<const wchar_t*, 1> extPaths = { wExtPath.c_str() };
        mPRTHandle.reset(prt::init(extPaths.data(), extPaths.size(), defaultLogLevel));
    }

    ~PRTContext() {
        // shutdown PRT
        mPRTHandle.reset();

        // remove loggers
        prt::removeLogHandler(mLogHandler.get());
        prt::removeLogHandler(mFileLogHandler.get());
    }

    explicit operator bool() const {
        return (bool)mPRTHandle;
    }

    pcu::ConsoleLogHandlerPtr mLogHandler;
    pcu::FileLogHandlerPtr    mFileLogHandler;
    pcu::ObjectPtr            mPRTHandle;
};

class Geometry {
public:
    Geometry(std::vector<double> vert);
    Geometry() { }
    ~Geometry() { }

    void setGeometry(std::vector<double> vert, size_t vertCnt, std::vector<uint32_t> ind, size_t indCnt, std::vector<uint32_t> faceCnt, size_t faceCntCnt);
    double* getVertices() { return vectorToArray(vertices); }
    size_t getVertexCount() { return vertexCount; }
    uint32_t* getIndices() { return vectorToArray(indices); }
    size_t getIndexCount() { return indexCount; }
    uint32_t* getFaceCounts() { return vectorToArray(faceCounts); }
    size_t getFaceCountsCount() { return faceCountsCount; }
    
protected:
    std::vector<double> vertices;
    size_t vertexCount;
    std::vector<uint32_t> indices;
    size_t indexCount;
    std::vector<uint32_t> faceCounts;
    size_t faceCountsCount;
};

Geometry::Geometry(std::vector<double> vert) {
    vertices = vert;
    vertexCount = vert.size();
    indexCount = (size_t)(vertexCount / 3);
    faceCountsCount = 1;

    std::vector<uint32_t> indicesVector(indexCount);
    std::iota(std::begin(indicesVector), std::end(indicesVector), 0);
    indices = indicesVector;
    std::vector<uint32_t> faceVector(1, (uint32_t)indexCount);
    faceCounts = faceVector;
}

void Geometry::setGeometry(std::vector<double> vert, size_t vertCnt, std::vector<uint32_t> ind, size_t indCnt, std::vector<uint32_t> faceCnt, size_t faceCntCnt) {
    vertices = vert;
    vertexCount = vertCnt;
    indices = ind;
    indexCount = indCnt;
    faceCounts = faceCnt;
    faceCountsCount = faceCntCnt;
}

class GeneratedGeometry {
public:
    GeneratedGeometry(std::map<int32_t,std::vector<std::vector<double>>> vertMatrix, std::map<int32_t,std::vector<std::vector<uint32_t>>> fMatrix, std::map<int32_t, FloatMap> floatRep, std::map<int32_t, StringMap> stringRep, std::map<int32_t, BoolMap> boolRep);
    GeneratedGeometry() {}
    ~GeneratedGeometry() {}

    std::map<int32_t, std::vector<std::vector<double>>> getGenerationVertices() { return verticesMatrix; }
    std::map<int32_t, std::vector<std::vector<uint32_t>>> getGenerationFaces() { return facesMatrix; }
    std::map<int32_t, FloatMap> getGenerationFloatReport() { return floatReport; }
    std::map<int32_t, StringMap> getGenerationStringReport() { return stringReport; }
    std::map<int32_t, BoolMap> getGenerationBoolReport() { return boolReport; }

private:
    std::map<int32_t, std::vector<std::vector<double>>> verticesMatrix;
    std::map<int32_t, std::vector<std::vector<uint32_t>>> facesMatrix;
    std::map<int32_t, FloatMap> floatReport;
    std::map<int32_t, StringMap> stringReport;
    std::map<int32_t, BoolMap> boolReport;
};

GeneratedGeometry::GeneratedGeometry(std::map<int32_t, std::vector<std::vector<double>>> vertMatrix, std::map<int32_t, std::vector<std::vector<uint32_t>>> fMatrix, std::map<int32_t, FloatMap> floatRep, std::map<int32_t, StringMap> stringRep, std::map<int32_t, BoolMap> boolRep) {
    verticesMatrix = vertMatrix;
    facesMatrix = fMatrix;
    floatReport = floatRep;
    stringReport = stringRep;
    boolReport = boolRep;
}

namespace {
    class ModelGenerator {
    public:
        ModelGenerator(const std::string& initShapePath);
        ModelGenerator(const std::vector<Geometry>& myGeo);
        ~ModelGenerator();
        
        static void initializePRT(std::string const & prtPath = "");
        static void shutdownPRT();
        static bool isPRTInitialized();
        std::vector<GeneratedGeometry> generateModel(const std::string& rulePackagePath, const std::vector<std::string>& shapeAttributes, const wchar_t* encoderName, const std::vector<std::string>& encoderOptions);
        
        bool isCustomGeometry() { return customFlag; }

    private:
        std::string initialShapePath;
        std::vector<Geometry> initialGeometries;

        static std::unique_ptr<PRTContext> prtCtx;

        bool customFlag = false;
    };

    std::unique_ptr<PRTContext> ModelGenerator::prtCtx = nullptr;

    ModelGenerator::ModelGenerator(const std::string& initShapePath) {
        initialShapePath = initShapePath;
    }

    ModelGenerator::ModelGenerator(const std::vector<Geometry>& myGeo) {
        initialGeometries = myGeo;
        customFlag = true;
    }

    ModelGenerator::~ModelGenerator() {
    }

    void ModelGenerator::initializePRT(std::string const & prtPath) {
        if (!prtCtx) prtCtx.reset(new PRTContext((prt::LogLevel) 1, prtPath));
    }

    bool ModelGenerator::isPRTInitialized() {
        return prtCtx != nullptr;
    }

    void ModelGenerator::shutdownPRT() {
        prtCtx.reset();
    }

    std::vector<GeneratedGeometry> ModelGenerator::generateModel(const std::string& rulePackagePath, const std::vector<std::string>& shapeAttributes, const wchar_t* encoderName = ENCODER_ID_PYTHON, const std::vector<std::string>& encoderOptions = {"baseName:string=theModel"}) {
        std::vector<GeneratedGeometry> generatedGeometries;

        try {
            // Step 1: Initialization (setup console, logging, licensing information, PRT extension library path, prt::init)
            if (!prtCtx) {
                LOG_ERR << L"prt has not been initialized." << std::endl;
                return {};
            }


            // Step 2: Resolve Map, Callbacks, Cache
            pcu::ResolveMapPtr resolveMap;

            if (!rulePackagePath.empty()) {
                LOG_INF << "Using rule package " << rulePackagePath << std::endl;

                const std::string u8rpkURI = pcu::toFileURI(rulePackagePath);
                prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
                try {
                    auto* r = prt::createResolveMap(pcu::toUTF16FromUTF8(u8rpkURI).c_str(), nullptr, &status);
                    resolveMap.reset(r);
                }
                catch (std::exception e) {
                    std::cerr << e.what() << std::endl;
                }


                if (resolveMap && (status == prt::STATUS_OK)) {
                    LOG_DBG << "resolve map = " << pcu::objectToXML(resolveMap.get()) << std::endl;
                }
                else {
                    LOG_ERR << "getting resolve map from '" << rulePackagePath << "' failed, aborting." << std::endl;
                    return {};
                }
            }


            // -- create cache
            pcu::CachePtr cache{ prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT) };


            // -- setup initial shape attributes
            std::wstring       ruleFile = L"bin/rule.cgb";
            std::wstring       startRule = L"default$init";
            int32_t            seed = 666;
            const std::wstring shapeName = L"TheInitialShape";

            const pcu::AttributeMapPtr convertedShapeAttr{ pcu::createAttributeMapFromTypedKeyValues(shapeAttributes) };
            if (convertedShapeAttr) {
                if (convertedShapeAttr->hasKey(L"ruleFile") &&
                    convertedShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
                    ruleFile = convertedShapeAttr->getString(L"ruleFile");
                if (convertedShapeAttr->hasKey(L"startRule") &&
                    convertedShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
                    startRule = convertedShapeAttr->getString(L"startRule");
            }


            // Step 4 : Generate (encoder info, encoder options, trigger procedural 3D model generation)
            const pcu::AttributeMapBuilderPtr optionsBuilder{ prt::AttributeMapBuilder::create() };
            optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
            const pcu::AttributeMapPtr reportOptions{ optionsBuilder->createAttributeMapAndReset() };
            const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapFromTypedKeyValues(encoderOptions) };


            // -- validate & complete encoder options
            const pcu::AttributeMapPtr validatedReportOpts{ createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions) };
            const pcu::AttributeMapPtr validatedEncOpts{ createValidatedOptions(encoderName, encOptions) };


            //-- setup encoder IDs and corresponding options
            const std::array<const wchar_t*, 2> encoders = {
                    encoderName,
                    ENCODER_ID_CGA_REPORT, // an encoder to redirect CGA report to CGAReport.txt
            };
            const std::array<const prt::AttributeMap*, 2> encoderOpts = { validatedEncOpts.get(), validatedReportOpts.get() };


            if (isCustomGeometry()) {
                for (Geometry myGeometry : initialGeometries) {

                    // Step 3: Initial Shape
                    pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };
                    if(isb->setGeometry(
                            myGeometry.getVertices(), myGeometry.getVertexCount(),
                            myGeometry.getIndices(), myGeometry.getIndexCount(),
                            myGeometry.getFaceCounts(), myGeometry.getFaceCountsCount()) != prt::STATUS_OK) {
             
                        isb->setGeometry(
                            pcu::quad::vertices, pcu::quad::vertexCount,
                            pcu::quad::indices, pcu::quad::indexCount,
                            pcu::quad::faceCounts, pcu::quad::faceCountsCount
                        );
                    }


                    isb->setAttributes(
                        ruleFile.c_str(),
                        startRule.c_str(),
                        seed,
                        shapeName.c_str(),
                        convertedShapeAttr.get(),
                        resolveMap.get()
                    );


                    // -- create initial shape
                    const pcu::InitialShapePtr initialShape{ isb->createInitialShapeAndReset() };
                    const std::vector<const prt::InitialShape*> initialShapes = { initialShape.get() };

                    if (!std::wcscmp(encoderName, ENCODER_ID_PYTHON)) {
                        pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>() };

                        // Step 5: Generate
                        const prt::Status genStat = prt::generate(
                            initialShapes.data(), initialShapes.size(), nullptr,
                            encoders.data(), encoders.size(), encoderOpts.data(),
                            foc.get(), cache.get(), nullptr
                        );

                        if (genStat != prt::STATUS_OK) {
                            LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                            return {};
                        }

                        GeneratedGeometry newGeneratedGeo(foc->getVertices(), foc->getFaces(), foc->getFloatReport(), foc->getStringReport(), foc->getBoolReport());
                        generatedGeometries.push_back(newGeneratedGeo);
                    }
                    else {
                        const pcu::Path output_path = executablePath.getParent().getParent() / "output";
                        if (!output_path.exists()) {
                            _mkdir(output_path.generic_string().c_str());
                            LOG_INF << "New output directory created at " << output_path << std::endl;
                        }

                        pcu::FileOutputCallbacksPtr foc{ prt::FileOutputCallbacks::create(output_path.native_wstring().c_str()) };

                        // Step 5: Generate
                        const prt::Status genStat = prt::generate(
                            initialShapes.data(), initialShapes.size(), nullptr,
                            encoders.data(), encoders.size(), encoderOpts.data(),
                            foc.get(), cache.get(), nullptr
                        );

                        if (genStat != prt::STATUS_OK) {
                            LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                            return {};
                        }

                        return {};
                    }

                }
            }
            else {
                pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

                if (!pcu::toFileURI(initialShapePath).empty()) {
                    LOG_DBG << L"trying to read initial shape geometry from " << pcu::toFileURI(initialShapePath);
                    const prt::Status s = isb->resolveGeometry(pcu::toUTF16FromOSNarrow(pcu::toFileURI(initialShapePath)).c_str(), resolveMap.get(), cache.get());
                    if (s != prt::STATUS_OK) {
                        LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(initialShapePath);
                        return {};
                    }
                }
                else {
                    isb->setGeometry(
                        pcu::quad::vertices, pcu::quad::vertexCount,
                        pcu::quad::indices, pcu::quad::indexCount,
                        pcu::quad::faceCounts, pcu::quad::faceCountsCount
                    );
                }

                isb->setAttributes(
                    ruleFile.c_str(),
                    startRule.c_str(),
                    seed,
                    shapeName.c_str(),
                    convertedShapeAttr.get(),
                    resolveMap.get()
                );

                // -- create initial shape
                const pcu::InitialShapePtr initialShape{ isb->createInitialShapeAndReset() };
                const std::vector<const prt::InitialShape*> initialShapes = { initialShape.get() };

                if (!std::wcscmp(encoderName, ENCODER_ID_PYTHON)) {
                    pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>() };

                    // Step 5: Generate
                    const prt::Status genStat = prt::generate(
                        initialShapes.data(), initialShapes.size(), nullptr,
                        encoders.data(), encoders.size(), encoderOpts.data(),
                        foc.get(), cache.get(), nullptr
                    );

                    if (genStat != prt::STATUS_OK) {
                        LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                        return {};
                    }

                    GeneratedGeometry newGeneratedGeo(foc->getVertices(), foc->getFaces(), foc->getFloatReport(), foc->getStringReport(), foc->getBoolReport());
                    generatedGeometries.push_back(newGeneratedGeo);
                }
                else {
                    const pcu::Path output_path = executablePath.getParent().getParent() / "output";
                    if (!output_path.exists()) {
                        _mkdir(output_path.generic_string().c_str());
                        LOG_INF << "New output directory created at " << output_path << std::endl;
                    }

                    pcu::FileOutputCallbacksPtr foc{ prt::FileOutputCallbacks::create(output_path.native_wstring().c_str()) };

                    // Step 5: Generate
                    const prt::Status genStat = prt::generate(
                        initialShapes.data(), initialShapes.size(), nullptr,
                        encoders.data(), encoders.size(), encoderOpts.data(),
                        foc.get(), cache.get(), nullptr
                    );

                    if (genStat != prt::STATUS_OK) {
                        LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
                        return {};
                    }

                    return {};
                }

            }

        }
        catch (const std::exception& e) {
            std::cerr << "caught exception: " << e.what() << std::endl;
            return {};
        }
        catch (...) {
            std::cerr << "caught unknown exception." << std::endl;
            return {};
        }
            

        return generatedGeometries;
    }

} // namespace

int py_printVal(int val) {
    return val;
}

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(pyprt, m) {
    py::class_<ModelGenerator>(m, "ModelGenerator")
        .def(py::init<const std::string&>(), "initShapePath"_a)
        .def(py::init<const std::vector<Geometry>&>(), "initShape"_a)
        .def("generate_model", &ModelGenerator::generateModel, py::arg("rulePackagePath"), py::arg("shapeAttributes"), py::arg("encoderName") = ENCODER_ID_PYTHON, py::arg("encoderOptions") = std::vector<std::string>(1, "baseName:string=theModel"));

    m.def("initialize_prt", &ModelGenerator::initializePRT, "prt_path"_a = "")
     .def("shutdown_prt", &ModelGenerator::shutdownPRT)
     .def("is_prt_initialized", &ModelGenerator::isPRTInitialized);

    py::class_<Geometry>(m, "Geometry")
        .def(py::init<>())
        .def(py::init<std::vector<double>>())
        .def("set_geometry",&Geometry::setGeometry)
        .def("get_vertices", &Geometry::getVertices)
        .def("get_vertex_count", &Geometry::getVertexCount)
        .def("get_indices", &Geometry::getIndices)
        .def("get_index_count", &Geometry::getIndexCount)
        .def("get_face_counts", &Geometry::getFaceCounts)
        .def("get_face_counts_count", &Geometry::getFaceCountsCount);

    py::class_<GeneratedGeometry>(m, "GeneratedGeometry")
        .def(py::init<std::map<int32_t, std::vector<std::vector<double>>>, std::map<int32_t, std::vector<std::vector<uint32_t>>>, std::map<int32_t, FloatMap>, std::map<int32_t, StringMap>, std::map<int32_t, BoolMap>>())
        .def("get_vertices", &GeneratedGeometry::getGenerationVertices)
        .def("get_faces", &GeneratedGeometry::getGenerationFaces)
        .def("get_float_report", &GeneratedGeometry::getGenerationFloatReport)
        .def("get_string_report", &GeneratedGeometry::getGenerationStringReport)
        .def("get_bool_report", &GeneratedGeometry::getGenerationBoolReport);

    m.def("print_val",&py_printVal,"Test Python function for value printing.");
}