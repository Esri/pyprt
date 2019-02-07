/**
 * Esri CityEngine SDK - Python Bindings
 * 
 * author: Camille Lechot
 */

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

namespace {

    /**
     * commonly used constants
     */
    const char*    FILE_LOG = "pyprt.log";
    const wchar_t* FILE_CGA_ERROR = L"CGAErrors.txt";
    const wchar_t* FILE_CGA_PRINT = L"CGAPrint.txt";
    const wchar_t* FILE_CGA_REPORT = L"CGAReport.txt";
    const char*    FILE_NAME_CGA_REPORT = "CGAReport.txt";
    const wchar_t* ENCODER_ID_CGA_ERROR = L"com.esri.prt.core.CGAErrorEncoder";
    const wchar_t* ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
    const wchar_t* ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
    const wchar_t* ENCODER_ID_PYTHON = L"com.esri.prt.examples.PyEncoder";
    const wchar_t* ENCODER_OPT_NAME = L"name";
    

    /**
     * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
     */
    struct PRTContext {
        PRTContext(prt::LogLevel defaultLogLevel) {
            //const pcu::Path executablePath(pcu::getExecutablePath());
            const pcu::Path executablePath("C:\\Users\\cami9495\\Documents\\esri-cityengine-sdk-master\\examples\\py4prt\\install\\bin");
            const pcu::Path installPath = executablePath.getParent();
            const pcu::Path fsLogPath = installPath / FILE_LOG;

            //mLogHandler.reset(prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT));
            mFileLogHandler.reset(prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT, fsLogPath.native_wstring().c_str()));
            //prt::addLogHandler(mLogHandler.get());
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
            //prt::removeLogHandler(mLogHandler.get());
            prt::removeLogHandler(mFileLogHandler.get());
        }

        explicit operator bool() const {
            return (bool)mPRTHandle;
        }

        //pcu::ConsoleLogHandlerPtr mLogHandler;
        pcu::FileLogHandlerPtr    mFileLogHandler;
        pcu::ObjectPtr            mPRTHandle;
    };

} // namespace

PRTContext prtCtx((prt::LogLevel) 0);

std::vector<std::vector<double>> prtInit(const std::string initShapePath, const std::string rulePath, const std::vector<std::string> shapeAttr, const std::vector<std::string> encoderOptions) {
    pcu::PyCallbacksPtr foc;
    try {
        // Step 1: Initialization (setup console, logging, licensing information, PRT extension library path, prt::init)
        if (!prtCtx) {
            LOG_ERR << L"failed to get a CityEngine license, bailing out." << std::endl;
        }


        // Step 2: Resolve Map, Callbacks, Cache
        pcu::ResolveMapPtr resolveMap;

        if (!rulePath.empty()) {
            LOG_INF << "Using rule package " << rulePath << std::endl;

            const std::string u8rpkURI = pcu::toFileURI(rulePath);
            prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
            try {
                auto* r = prt::createResolveMap(pcu::toUTF16FromUTF8(u8rpkURI).c_str(), nullptr, &status);
                resolveMap.reset(r);
            }
            catch (std::exception e) {
                std::cout << e.what() << std::endl;
            }


            if (resolveMap && (status == prt::STATUS_OK)) {
                LOG_DBG << "resolve map = " << pcu::objectToXML(resolveMap.get()) << std::endl;
            }
            else {
                LOG_ERR << "getting resolve map from '" << rulePath << "' failed, aborting." << std::endl;
            }
        }


        // -- create cache & callback
        foc = std::make_unique<PyCallbacks>();
        pcu::CachePtr cache{ prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT) };


        // Step 3: Initial Shape
        pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };
        if (!pcu::toFileURI(initShapePath).empty()) {
            LOG_DBG << L"trying to read initial shape geometry from " << pcu::toFileURI(initShapePath);
            const prt::Status s = isb->resolveGeometry(pcu::toUTF16FromOSNarrow(pcu::toFileURI(initShapePath)).c_str(), resolveMap.get(), cache.get());
            if (s != prt::STATUS_OK) {
                LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(initShapePath);
            }
        }
        else {
            isb->setGeometry(
                pcu::quad::vertices, pcu::quad::vertexCount,
                pcu::quad::indices, pcu::quad::indexCount,
                pcu::quad::faceCounts, pcu::quad::faceCountsCount
            );
        }


        // -- setup initial shape attributes
        std::wstring       ruleFile = L"bin/rule.cgb";
        std::wstring       startRule = L"default$init";
        int32_t            seed = 666;
        const std::wstring shapeName = L"TheInitialShape";

        const pcu::AttributeMapPtr convertedShapeAttr{ pcu::createAttributeMapFromTypedKeyValues(shapeAttr) };
        if (convertedShapeAttr) {
            if (convertedShapeAttr->hasKey(L"ruleFile") &&
                convertedShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
                ruleFile = convertedShapeAttr->getString(L"ruleFile");
            if (convertedShapeAttr->hasKey(L"startRule") &&
                convertedShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
                startRule = convertedShapeAttr->getString(L"startRule");
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


        // Step 4 : Generate (encoder info, encoder options, trigger procedural 3D model generation)
        const pcu::AttributeMapBuilderPtr optionsBuilder{ prt::AttributeMapBuilder::create() };
        optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_ERROR);
        const pcu::AttributeMapPtr errOptions{ optionsBuilder->createAttributeMapAndReset() };
        optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_PRINT);
        const pcu::AttributeMapPtr printOptions{ optionsBuilder->createAttributeMapAndReset() };
        optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
        const pcu::AttributeMapPtr reportOptions{ optionsBuilder->createAttributeMapAndReset() };
        const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapFromTypedKeyValues(encoderOptions) };


        // -- validate & complete encoder options
        const pcu::AttributeMapPtr validatedEncOpts{ createValidatedOptions(ENCODER_ID_PYTHON, encOptions) };
        const pcu::AttributeMapPtr validatedErrOpts{ createValidatedOptions(ENCODER_ID_CGA_ERROR, errOptions) };
        const pcu::AttributeMapPtr validatedPrintOpts{ createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions) };
        const pcu::AttributeMapPtr validatedReportOpts{ createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions) };


        // -- setup encoder IDs and corresponding options
        const std::array<const wchar_t*, 4> encoders = {
                ENCODER_ID_PYTHON,      // Python geometry encoder
                ENCODER_ID_CGA_ERROR, // an encoder to redirect rule errors to CGAErrors.txt
                ENCODER_ID_CGA_PRINT,  // an encoder to redirect CGA print statements to CGAPrint.txt
                ENCODER_ID_CGA_REPORT, // an encoder to redirect CGA report to CGAReport.txt
        };
        const std::array<const prt::AttributeMap*, 4> encoderOpts = { validatedEncOpts.get(), validatedErrOpts.get(), validatedPrintOpts.get(), validatedReportOpts.get() };


        // Step 5: Generate
        const prt::Status genStat = prt::generate(
            initialShapes.data(), initialShapes.size(), nullptr,
            encoders.data(), encoders.size(), encoderOpts.data(),
            foc.get(), cache.get(), nullptr
        );

        if (genStat != prt::STATUS_OK) {
            LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "caught exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "caught unknown exception." << std::endl;
    }
    
    return foc->getGeometry();
}


int py_printVal(int val) {
    //std::cout << val << std::endl;
    return val;
}

/*std::vector<std::string> py_prtReadReportGenerate(const std::string theRulePckPath, const std::string theRuleFile, const std::string theStartRule, const std::string theShapePath, const std::string outputFolder) {

    std::vector<std::string> reportRead;
    try {
        // Step 1: Initialization (setup console, logging, licensing information, PRT extension library path, prt::init)
        char *argvInit[] = { "","-l","8","-p",const_cast<char *>(theRulePckPath.c_str()),"-a",const_cast<char *>(theRuleFile.c_str()),"-a",const_cast<char *>(theStartRule.c_str()),"-e","com.esri.prt.codecs.OBJEncoder","-z","baseName:string=theBuilding","-o",const_cast<char *>(outputFolder.c_str()) };
        int argcInit = (int)(sizeof(argvInit) / sizeof(argvInit[0]));

        pcu::InputArgs inputArgs(argcInit, argvInit);
        if (!inputArgs.readyToRumble()) {
            std::cout << static_cast<int>(inputArgs.mStatus) << std::endl;
        }

        inputArgs.mInitialShapeGeo = pcu::toFileURI(theShapePath);

        if (!prtCtx) {
            std::cout << L"failed to get a CityEngine license, bailing out." << std::endl;
        }

        // -- handle the info option (must happen after successful init)
        if (!inputArgs.mInfoFile.empty()) {
            const pcu::RunStatus s = pcu::codecInfoToXML(inputArgs.mInfoFile);
            std::cout << static_cast<int>(s) << std::endl;
        }

        // Step 2: Resolve Map, Callbacks, Cache
        pcu::ResolveMapPtr resolveMap;

        if (!inputArgs.mRulePackage.empty()) {
            std::cout << "Using rule package " << inputArgs.mRulePackage << std::endl;

            const std::string u8rpkURI = pcu::toFileURI(inputArgs.mRulePackage);
            prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
            try {
                auto* r = prt::createResolveMap(pcu::toUTF16FromUTF8(u8rpkURI).c_str(), nullptr, &status);
                resolveMap.reset(r);
            }
            catch (std::exception e) {
                std::cout << e.what() << std::endl;
            }

            if (resolveMap && (status == prt::STATUS_OK)) {
                std::cout << "resolve map = " << pcu::objectToXML(resolveMap.get()) << std::endl;
            }
            else {
                std::cout << "getting resolve map from '" << inputArgs.mRulePackage << "' failed, aborting." << std::endl;
            }
        }

        // -- create cache & callback
        pcu::FileOutputCallbacksPtr foc{ prt::FileOutputCallbacks::create(inputArgs.mOutputPath.native_wstring().c_str()) };
        pcu::CachePtr cache{ prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT) };

        // Step 3: Initial Shape
        pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };
        if (!inputArgs.mInitialShapeGeo.empty()) {
            LOG_DBG << L"trying to read initial shape geometry from " << inputArgs.mInitialShapeGeo;
            const prt::Status s = isb->resolveGeometry(pcu::toUTF16FromOSNarrow(inputArgs.mInitialShapeGeo).c_str(), resolveMap.get(), cache.get());
            if (s != prt::STATUS_OK) {
                LOG_ERR << "could not resolve geometry from " << inputArgs.mInitialShapeGeo;
                std::cout << "Exit Failure" << std::endl;
            }
        }
        else {
            isb->setGeometry(
                pcu::quad::vertices, pcu::quad::vertexCount,
                pcu::quad::indices, pcu::quad::indexCount,
                pcu::quad::faceCounts, pcu::quad::faceCountsCount
            );
        }

        // -- setup initial shape attributes
        std::wstring       ruleFile = L"bin/rule.cgb";
        std::wstring       startRule = L"default$init";
        int32_t            seed = 666;
        const std::wstring shapeName = L"TheInitialShape";

        if (inputArgs.mInitialShapeAttrs) {
            if (inputArgs.mInitialShapeAttrs->hasKey(L"ruleFile") &&
                inputArgs.mInitialShapeAttrs->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
                ruleFile = inputArgs.mInitialShapeAttrs->getString(L"ruleFile");
            if (inputArgs.mInitialShapeAttrs->hasKey(L"startRule") &&
                inputArgs.mInitialShapeAttrs->getType(L"startRule") == prt::AttributeMap::PT_STRING)
                startRule = inputArgs.mInitialShapeAttrs->getString(L"startRule");
            if (inputArgs.mInitialShapeAttrs->hasKey(L"seed") &&
                inputArgs.mInitialShapeAttrs->getType(L"seed") == prt::AttributeMap::PT_INT)
                seed = inputArgs.mInitialShapeAttrs->getInt(L"seed");
        }

        isb->setAttributes(
            ruleFile.c_str(),
            startRule.c_str(),
            seed,
            shapeName.c_str(),
            inputArgs.mInitialShapeAttrs.get(),
            resolveMap.get()
        );

        // -- create initial shape
        const pcu::InitialShapePtr initialShape{ isb->createInitialShapeAndReset() };
        const std::vector<const prt::InitialShape*> initialShapes = { initialShape.get() };

        // Step 4 : Generate (encoder info, encoder options, trigger procedural 3D model generation)
        const pcu::AttributeMapBuilderPtr optionsBuilder{ prt::AttributeMapBuilder::create() };
        optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_ERROR);
        const pcu::AttributeMapPtr errOptions{ optionsBuilder->createAttributeMapAndReset() };
        optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_PRINT);
        const pcu::AttributeMapPtr printOptions{ optionsBuilder->createAttributeMapAndReset() };
        optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
        const pcu::AttributeMapPtr reportOptions{ optionsBuilder->createAttributeMapAndReset() };

        const pcu::AttributeMapPtr encOptions{ optionsBuilder->createAttributeMapAndReset() };

        // -- validate & complete encoder options
        const pcu::AttributeMapPtr validatedEncOpts{ createValidatedOptions(pcu::toUTF16FromOSNarrow(inputArgs.mEncoderID), inputArgs.mEncoderOpts) };
        const pcu::AttributeMapPtr validatedErrOpts{ createValidatedOptions(ENCODER_ID_CGA_ERROR, errOptions) };
        const pcu::AttributeMapPtr validatedPrintOpts{ createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions) };
        const pcu::AttributeMapPtr validatedReportOpts{ createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions) };

        // -- setup encoder IDs and corresponding options
        const std::wstring encoder = pcu::toUTF16FromOSNarrow(inputArgs.mEncoderID);
        const std::array<const wchar_t*, 4> encoders = {
                encoder.c_str(),      // our desired encoder
                ENCODER_ID_CGA_ERROR, // an encoder to redirect rule errors into CGAErrors.txt
                ENCODER_ID_CGA_PRINT,  // an encoder to redirect CGA print statements to CGAPrint.txt
                ENCODER_ID_CGA_REPORT // an encoder to redirect CGA report to CGAReport.txt
        };
        const std::array<const prt::AttributeMap*, 4> encoderOpts = { validatedEncOpts.get(), validatedErrOpts.get(), validatedPrintOpts.get(), validatedReportOpts.get() };


        // Step 5: Generate
        const prt::Status genStat = prt::generate(
            initialShapes.data(), initialShapes.size(), nullptr,
            encoders.data(), encoders.size(), encoderOpts.data(),
            foc.get(), cache.get(), nullptr
        );
        if (genStat != prt::STATUS_OK) {
            LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat << ")";
        }


        // Step 6: Read Report
        std::ifstream reportFile((inputArgs.mWorkDir / outputFolder / FILE_NAME_CGA_REPORT).native_string());
        std::string line;
        if (reportFile.is_open()) {
            while (getline(reportFile, line)) {
                reportRead.push_back(line);
            }
            reportFile.close();
            //std::cout << "File name to check: " << (inputArgs.mWorkDir / outputFolder / FILE_NAME_CGA_REPORT).native_string() << std::endl;
        }
        else std::cout << "Issue with file." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "caught exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "caught unknown exception." << std::endl;
    }

    return reportRead;
}*/

namespace {
    class ModelGenerator {
    public:
        ModelGenerator();
        ModelGenerator(std::string initShapePath, std::string rulePkgPath, std::vector<std::string> shapeAtt, std::vector<std::string> encOpt);
        ~ModelGenerator();

        bool generateModel();
        std::vector<std::vector<double>> getModelGeometry() const;

    private:
        std::string initialShapePath = "C:/Users/cami9495/Documents/esri-cityengine-sdk-master/data/simple_scene_0.obj";
        std::string rulePackagePath = "C:/Users/cami9495/Documents/esri-cityengine-sdk-master/data/simple_rule2019.rpk";
        std::vector<std::string> shapeAttributes = { "ruleFile:string=bin/simple_rule2019.cgb", "startRule:string=Default$Footprint" };
        std::vector<std::string> encoderOptions = { "baseName:string=theModelSuper" };

        std::vector<std::vector<double>> modelGeometry;
        
    };

    ModelGenerator::ModelGenerator() {
        std::cout << "Creation of default class instance" << std::endl;
    }

    ModelGenerator::ModelGenerator(std::string initShapePath, std::string rulePkgPath, std::vector<std::string> shapeAtt, std::vector<std::string> encOpt) {
        std::cout << "Creation of class instance" << std::endl;
        initialShapePath = initShapePath;
        rulePackagePath = rulePkgPath;
        shapeAttributes = shapeAtt;
        encoderOptions = encOpt;
    }

    ModelGenerator::~ModelGenerator() {
    }

    bool ModelGenerator::generateModel() {
        modelGeometry = prtInit(initialShapePath,rulePackagePath,shapeAttributes,encoderOptions);
        return true;
    }

    std::vector<std::vector<double>> ModelGenerator::getModelGeometry() const {
        return modelGeometry;
    }

} // namespace


namespace py = pybind11;

PYBIND11_MODULE(pyprt, m) {
    py::class_<ModelGenerator>(m, "ModelGenerator")
        .def(py::init<>()).def(py::init<std::string,std::string,std::vector<std::string>,std::vector<std::string>>())
        .def("generateModel", &ModelGenerator::generateModel)
        .def("getModelGeometry", &ModelGenerator::getModelGeometry);

    m.def("printVal",&py_printVal,"Test Python added function for printing.");
    //m.def("prtReportGenerate", &py_prtReadReportGenerate, "");
}