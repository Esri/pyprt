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
#include <filesystem>
#ifdef _WIN32
#	include <direct.h>
#endif

#include <ctime>

/**
  * commonly used constants
  */
const wchar_t* FILE_CGA_REPORT = L"CGAReport.txt";
const wchar_t* ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const wchar_t* ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
const wchar_t* ENCODER_ID_PYTHON = L"com.esri.prt.examples.PyEncoder";
const wchar_t* ENCODER_OPT_NAME = L"name";
pcu::Path executablePath;


template <typename T>
T* vectorToArray(const std::vector<T>& data) {
    size_t array_size = data.size();
    T* tmp = new T[array_size];

    for (int i = 0; i < (int)array_size; i++) {
        tmp[i] = data[i];
    }

    return tmp;
}

// cstr must have space for cstrSize characters
// cstr will be null-terminated and the actually needed size is placed in cstrSize
void copyToCStr(const std::string& str, char* cstr, size_t& cstrSize) {
	if (cstrSize > 0) {
		strncpy(cstr, str.c_str(), cstrSize);
		cstr[cstrSize - 1] = 0x0; // enforce null-termination
	}
	cstrSize = str.length()+1; // returns the actually needed size including terminating null
}

/**
  * custom console logger to redirect PRT log events into the python output
  */
class PythonLogHandler : public prt::LogHandler {
public:
	PythonLogHandler() = default;
	virtual ~PythonLogHandler() = default;

	virtual void handleLogEvent(const wchar_t* msg, prt::LogLevel /*level*/) {
		pybind11::print(L"[PRT]", msg);
	}

	virtual const prt::LogLevel* getLevels(size_t* count) {
		*count = prt::LogHandler::ALL_COUNT;
		return prt::LogHandler::ALL;
	}

	virtual void getFormat(bool* dateTime, bool* level) {
		*dateTime = true;
		*level = true;
	}

	virtual char* toXML(char* result, size_t* resultSize, prt::Status* stat = 0) const {
		std::ostringstream out;
		out << *this;
		copyToCStr(out.str(), result, *resultSize);
		if (stat) *stat = prt::STATUS_OK;
		return result;
	}

	friend std::ostream& operator<<(std::ostream& stream, const PythonLogHandler&) {
		stream << "<PythonLogHandler />";
		return stream;
	}

private:
	const prt::LogLevel* mLevels;
	size_t mCount;
};


/**
  * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
  */
struct PRTContext {
    PRTContext(prt::LogLevel minimalLogLevel, std::string const & sdkPath) {
        executablePath = sdkPath.empty() ? pcu::getExecutablePath() : sdkPath;
        const pcu::Path installPath = executablePath.getParent();

        prt::addLogHandler(&mLogHandler);

        // setup paths for plugins, assume standard SDK layout as per README.md
        const pcu::Path extPath = installPath / "lib";

        // initialize PRT with the path to its extension libraries, the default log level
        const std::wstring wExtPath = extPath.native_wstring();
        const std::array<const wchar_t*, 1> extPaths = { wExtPath.c_str() };
        mPRTHandle.reset(prt::init(extPaths.data(), extPaths.size(), minimalLogLevel));
    }

    ~PRTContext() {
        // shutdown PRT
        mPRTHandle.reset();

        // remove loggers
        prt::removeLogHandler(&mLogHandler);
    }

    explicit operator bool() const {
        return (bool)mPRTHandle;
    }

	PythonLogHandler mLogHandler;
    pcu::ObjectPtr mPRTHandle;
};

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
    //GeneratedGeometry(const uint32_t& initialShapeIdx, const std::vector<std::vector<double>>& vertMatrix, const std::vector<std::vector<uint32_t>>& fMatrix, const FloatMap& floatRep, const StringMap& stringRep, const BoolMap& boolRep);
    GeneratedGeometry(uint32_t initialShapeIdx, std::vector<std::vector<double>> vertMatrix, std::vector<std::vector<uint32_t>> fMatrix, FloatMap floatRep, StringMap stringRep, BoolMap boolRep);
    GeneratedGeometry() { }
    ~GeneratedGeometry() { }

    uint32_t getInitialShapeIndex() { return initialShapeIdx; }
    std::vector<std::vector<double>> getGenerationVertices() { return verticesMatrix; }
    std::vector<std::vector<uint32_t>> getGenerationFaces() { return facesMatrix; }
    FloatMap getGenerationFloatReport() { return floatReportMap; }
    StringMap getGenerationStringReport() { return stringReportMap; }
    BoolMap getGenerationBoolReport() { return boolReportMap; }

private:
    uint32_t initialShapeIdx;
    std::vector<std::vector<double>> verticesMatrix;
    std::vector<std::vector<uint32_t>> facesMatrix;
    FloatMap floatReportMap;
    StringMap stringReportMap;
    BoolMap boolReportMap;
};

//GeneratedGeometry::GeneratedGeometry(const uint32_t& initShapeIdx, const std::vector<std::vector<double>>& vertMatrix, const std::vector<std::vector<uint32_t>>& fMatrix, const FloatMap& floatRep, const StringMap& stringRep, const BoolMap& boolRep) {
GeneratedGeometry::GeneratedGeometry(uint32_t initShapeIdx, std::vector<std::vector<double>> vertMatrix, std::vector<std::vector<uint32_t>> fMatrix, FloatMap floatRep, StringMap stringRep, BoolMap boolRep) {
    initialShapeIdx = initShapeIdx;
    verticesMatrix = vertMatrix;
    facesMatrix = fMatrix;
    floatReportMap = floatRep;
    stringReportMap = stringRep;
    boolReportMap = boolRep;
}


namespace {
    class ModelGenerator {
    public:
        ModelGenerator(const std::string& initShapePath);
        ModelGenerator(const std::vector<Geometry>& myGeo);
        ~ModelGenerator() { }

        std::vector<GeneratedGeometry> generateModel(const std::string& rulePackagePath, py::dict shapeAttributes, py::dict encoderOptions, const wchar_t* encoderName);
        std::vector<GeneratedGeometry> generateAnotherModel(py::dict shapeAttributes, py::dict encoderOptions);

        bool isCustomGeometry() { return customFlag; }

    private:
        std::string initialShapePath;
        std::vector<Geometry> initialGeometries;
        bool customFlag = false;
        pcu::ResolveMapPtr resolveMap;
        pcu::CachePtr cache;

        pcu::AttributeMapBuilderPtr encoderBuilder;
        pcu::AttributeMapPtr CGAReportOptions;
        pcu::AttributeMapPtr CGAPrintOptions;
        pcu::AttributeMapPtr pyEncoderOptions;
        std::vector<const prt::AttributeMap*> allEncodersOptions;
        std::vector<const wchar_t*> allEncoders;

        std::vector<pcu::InitialShapeBuilderPtr> initialShapesBuilders;

        std::wstring ruleFile = L"bin/rule.cgb";
        std::wstring startRule = L"default$init";
        int32_t seed = 666;
        std::wstring shapeName = L"InitialShape";
    };

    ModelGenerator::ModelGenerator(const std::string& initShapePath) {
        initialShapePath = initShapePath;
        initialShapesBuilders.reserve(1);

        cache = (pcu::CachePtr) prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

        // Initial shape initializing
        pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

        if (!pcu::toFileURI(initialShapePath).empty()) {
            LOG_DBG << "trying to read initial shape geometry from " << pcu::toFileURI(initialShapePath) << std::endl;
            const prt::Status s = isb->resolveGeometry(pcu::toUTF16FromOSNarrow(pcu::toFileURI(initialShapePath)).c_str(), resolveMap.get(), cache.get());
            if (s != prt::STATUS_OK) {
                LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(initialShapePath);
            }
        }
        else {
            isb->setGeometry(
                pcu::quad::vertices, pcu::quad::vertexCount,
                pcu::quad::indices, pcu::quad::indexCount,
                pcu::quad::faceCounts, pcu::quad::faceCountsCount
            );
        }

        if (!initialShapesBuilders.size()) {
            initialShapesBuilders.push_back(std::move(isb));
        }
        else {
            initialShapesBuilders[0] = std::move(isb);
        }
    }

    ModelGenerator::ModelGenerator(const std::vector<Geometry>& myGeo) {
        initialGeometries = myGeo;
        customFlag = true;
        initialShapesBuilders.reserve(myGeo.size());

        cache = (pcu::CachePtr) prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

        // Initial shapes initializing
        for (size_t ind = 0; ind < initialGeometries.size(); ind++) {

            pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

            if (isb->setGeometry(
                initialGeometries[ind].getVertices(), initialGeometries[ind].getVertexCount(),
                initialGeometries[ind].getIndices(), initialGeometries[ind].getIndexCount(),
                initialGeometries[ind].getFaceCounts(), initialGeometries[ind].getFaceCountsCount()) != prt::STATUS_OK) {

                isb->setGeometry(
                    pcu::quad::vertices, pcu::quad::vertexCount,
                    pcu::quad::indices, pcu::quad::indexCount,
                    pcu::quad::faceCounts, pcu::quad::faceCountsCount
                );
            }

            if (initialShapesBuilders.size() <= ind) {
                initialShapesBuilders.push_back(std::move(isb));
            }
            else {
                initialShapesBuilders[ind] = std::move(isb);
            }

        }
    }

    std::vector<GeneratedGeometry> ModelGenerator::generateModel(const std::string& rulePackagePath,
            py::dict shapeAttributes,
            py::dict encoderOptions = {},
            const wchar_t* encoderName = ENCODER_ID_PYTHON)
    {

        std::clock_t start;
        double duration;
        start = std::clock();

        std::vector<GeneratedGeometry> newGeneratedGeo;
        newGeneratedGeo.reserve(initialShapesBuilders.size());


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
            pcu::AttributeMapBuilderPtr shapeBld{ prt::AttributeMapBuilder::create() };
            const pcu::AttributeMapPtr convertedShapeAttr{ pcu::createAttributeMapFromPythonDict(shapeAttributes, shapeBld) };
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

            pcu::AttributeMapBuilderPtr bld{ prt::AttributeMapBuilder::create() };
            encoderBuilder = std::move(bld);
            const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapFromPythonDict(encoderOptions, encoderBuilder) };

            CGAReportOptions = createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions);
            CGAPrintOptions = createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions);
            pyEncoderOptions = createValidatedOptions(encoderName, encOptions);

            if (!allEncoders.empty()) { 
                delete allEncoders[0];
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
            //initialShapePtrs.reserve(initialShapesBuilders.size());
            std::vector<const prt::InitialShape*> initialShapes;
            //initialShapes.reserve(initialShapesBuilders.size());

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
                pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>() };

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

                for (size_t i = 0; i < initialShapesBuilders.size(); i++) {
                    uint32_t theIndex = foc->getInitialShapeIndex(i);
                    GeneratedGeometry geo(theIndex, foc->getVertices(theIndex), foc->getFaces(theIndex), foc->getFloatReport(theIndex), foc->getStringReport(theIndex), foc->getBoolReport(theIndex));
                    newGeneratedGeo.push_back(geo);
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
        std::clock_t start;
        double duration;
        start = std::clock();

        std::vector<GeneratedGeometry> newGeneratedGeo;
        newGeneratedGeo.reserve(initialShapesBuilders.size());

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
            pcu::AttributeMapBuilderPtr shapeBld{ prt::AttributeMapBuilder::create() };
            const pcu::AttributeMapPtr convertedShapeAttr{ pcu::createAttributeMapFromPythonDict(shapeAttributes, shapeBld) };
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
            //initialShapePtrs.reserve(initialShapesBuilders.size());
            std::vector<const prt::InitialShape*> initialShapes;
            //initialShapes.reserve(initialShapesBuilders.size());

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

                pcu::PyCallbacksPtr foc{ std::make_unique<PyCallbacks>() };

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

                //newGeneratedGeo = GeneratedGeometry(foc->getVertices(), foc->getFaces(), foc->getFloatReport(), foc->getStringReport(), foc->getBoolReport());
                for (size_t i = 0; i < initialShapesBuilders.size(); i++) {
                    uint32_t theIndex = foc->getInitialShapeIndex(i);
                    GeneratedGeometry geo(theIndex, foc->getVertices(theIndex), foc->getFaces(theIndex), foc->getFloatReport(theIndex), foc->getStringReport(theIndex), foc->getBoolReport(theIndex));
                    newGeneratedGeo.push_back(geo);
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


void py_testprintdict(py::dict dict) {
    for (auto item : dict) {
        std::cout << "key=" << std::string(py::str(item.first)) << ", " << "value=" << std::string(py::str(item.second)) << std::endl;

        if (py::isinstance<py::str>(item.first)) {
            std::cout << "OK" << std::endl;
            std::wstring salut = item.first.cast<std::wstring>();
            std::wcout << L"Ceci est un wstring: " << salut << std::endl;
        }

        if (py::isinstance<py::list>(item.second.ptr())) {
            std::cout << "It's a list!" << std::endl;
            auto lll = item.second.cast<py::list>();
            
            if (py::isinstance<py::bool_>(lll[0]))
                std::cout << "List of bool of size: " << lll.size() << std::endl;
            else if (py::isinstance<py::float_>(lll[0]))
                std::cout << "List of float of size: " << lll.size() << std::endl;
            else if (py::isinstance<py::int_>(lll[0]))
                std::cout << "List of int of size: " << lll.size() << std::endl;
            else if (py::isinstance<py::str>(lll[0]))
                std::cout << "List of string of size: " << lll.size() << std::endl;
            else
                std::cout << "Unknown list type." << std::endl;
        }
        else {
            if (py::isinstance<py::bool_>(item.second.ptr())) // check for boolean at first!!
                std::cout << "Instance of bool." << std::endl;
            else if (py::isinstance<py::float_>(item.second.ptr()))
                std::cout << "Instance of float." << std::endl;
            else if (py::isinstance<py::int_>(item.second.ptr()))
                std::cout << "Instance of int." << std::endl;
            else if (py::isinstance<py::str>(item.second.ptr()))
                std::cout << "Instance of string." << std::endl;
            else
                std::cout << "Unknown type." << std::endl;
        }
    }
}

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
        .def("set_geometry",&Geometry::setGeometry)
        .def("get_vertices", &Geometry::getVertices)
        .def("get_vertex_count", &Geometry::getVertexCount)
        .def("get_indices", &Geometry::getIndices)
        .def("get_index_count", &Geometry::getIndexCount)
        .def("get_face_counts", &Geometry::getFaceCounts)
        .def("get_face_counts_count", &Geometry::getFaceCountsCount);

    py::class_<GeneratedGeometry>(m, "GeneratedGeometry")
        //.def(py::init<const uint32_t&, const std::vector<std::vector<double>>&, const std::vector<std::vector<uint32_t>>&, const FloatMap&, const StringMap&, const BoolMap&>())
        .def(py::init<uint32_t, std::vector<std::vector<double>>, std::vector<std::vector<uint32_t>>, FloatMap, StringMap, BoolMap>())
        .def("get_initial_shape_index", &GeneratedGeometry::getInitialShapeIndex)
        .def("get_vertices", &GeneratedGeometry::getGenerationVertices)
        .def("get_faces", &GeneratedGeometry::getGenerationFaces)
        .def("get_float_report", &GeneratedGeometry::getGenerationFloatReport)
        .def("get_string_report", &GeneratedGeometry::getGenerationStringReport)
        .def("get_bool_report", &GeneratedGeometry::getGenerationBoolReport);

    m.def("print_val", &py_printVal,"Test Python function for value printing.");
    m.def("print_dict", &py_testprintdict, "Test function for dictionary printing.");
}