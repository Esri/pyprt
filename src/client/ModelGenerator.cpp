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

#include "ModelGenerator.h"
#include "PRTContext.h"
#include "logging.h"

namespace {

const wchar_t* FILE_CGA_REPORT = L"CGAReport.txt";
const wchar_t* ENCODER_OPT_NAME = L"name";
constexpr const char* ENC_OPT_OUTPUT_PATH = "outputPath";

const std::wstring ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const std::wstring ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
const std::wstring ENCODER_ID_CGA_ERROR = L"com.esri.prt.core.CGAErrorEncoder";
const std::wstring ENCODER_ID_PYTHON = L"com.esri.pyprt.PyEncoder";

void extractMainShapeAttributes(const py::dict& shapeAttr, int32_t& seed, std::wstring& shapeName,
                                pcu::AttributeMapPtr& convertShapeAttr) {
	convertShapeAttr = pcu::createAttributeMapFromPythonDict(
	        shapeAttr, *(pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));
	if (convertShapeAttr) {
		if (convertShapeAttr->hasKey(L"seed") && convertShapeAttr->getType(L"seed") == prt::AttributeMap::PT_INT)
			seed = convertShapeAttr->getInt(L"seed");
		if (convertShapeAttr->hasKey(L"shapeName") &&
		    convertShapeAttr->getType(L"shapeName") == prt::AttributeMap::PT_STRING)
			shapeName = convertShapeAttr->getString(L"shapeName");
	}
}

std::wstring detectStartRule(const pcu::RuleFileInfoUPtr& ruleFileInfo) {
	for (size_t r = 0; r < ruleFileInfo->getNumRules(); r++) {
		const auto* rule = ruleFileInfo->getRule(r);

		// start rules must not have any parameters
		if (rule->getNumParameters() > 0)
			continue;

		for (size_t a = 0; a < rule->getNumAnnotations(); a++) {
			if (std::wcscmp(rule->getAnnotation(a)->getName(), L"@StartRule") == 0) {
				return rule->getName();
			}
		}
	}
	return {};
}

} // namespace

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

		int32_t randomS = mSeed;
		std::wstring shapeN = mShapeName;
		extractMainShapeAttributes(shapeAttr, randomS, shapeN, convertedShapeAttr[ind]);

		mInitialShapesBuilders[ind]->setAttributes(mRuleFile.c_str(), mStartRule.c_str(), randomS, shapeN.c_str(),
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
		mEncodersNames.push_back(ENCODER_ID_CGA_ERROR);

		const pcu::AttributeMapBuilderPtr optionsBuilder{prt::AttributeMapBuilder::create()};
		optionsBuilder->setString(ENCODER_OPT_NAME, FILE_CGA_REPORT);
		const pcu::AttributeMapPtr reportOptions{optionsBuilder->createAttributeMapAndReset()};
		const pcu::AttributeMapPtr printOptions{optionsBuilder->createAttributeMapAndReset()};
		const pcu::AttributeMapPtr errorOptions{optionsBuilder->createAttributeMapAndReset()};

		mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions));
		mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions));
		mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_CGA_ERROR, errorOptions));
	}
}

void ModelGenerator::getRawEncoderDataPointers(std::vector<const prt::AttributeMap*>& allEncOpt) {
	if (mEncodersNames[0] == ENCODER_ID_PYTHON) {
		allEncOpt.clear();

		allEncOpt.push_back(mEncodersOptionsPtr[0].get());
	}
	else {
		allEncOpt.clear();
		allEncOpt.push_back(mEncodersOptionsPtr[0].get());
		allEncOpt.push_back(mEncodersOptionsPtr[1].get());
		allEncOpt.push_back(mEncodersOptionsPtr[2].get());
		allEncOpt.push_back(mEncodersOptionsPtr[3].get());
	}
}

std::vector<GeneratedModel> ModelGenerator::generateModel(const std::vector<py::dict>& shapeAttributes,
                                                          const std::filesystem::path& rulePackagePath,
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
		if (!PRTContext::get()) {
			LOG_ERR << "PRT has not been initialized.";
			return {};
		}

		if (!pcu::getResolveMap(rulePackagePath, &mResolveMap))
			return {};

		mRuleFile = pcu::getRuleFileEntry(mResolveMap.get());

		const wchar_t* ruleFileURI = mResolveMap->getString(mRuleFile.c_str());
		if (ruleFileURI == nullptr) {
			LOG_ERR << "could not find rule file URI in resolve map of rule package " << rulePackagePath;
			return {};
		}

		prt::Status infoStatus = prt::STATUS_UNSPECIFIED_ERROR;
		pcu::RuleFileInfoUPtr info(prt::createRuleFileInfo(ruleFileURI, mCache.get(), &infoStatus));
		if (!info || infoStatus != prt::STATUS_OK) {
			LOG_ERR << "could not get rule file info from rule file " << mRuleFile;
			return {};
		}

		mStartRule = detectStartRule(info);

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

		std::vector<const wchar_t*> encoders = pcu::toPtrVec(mEncodersNames);

		std::vector<const prt::AttributeMap*> encodersOptions;
		encodersOptions.reserve(3);
		getRawEncoderDataPointers(encodersOptions);

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
			const std::filesystem::path outputPath = [&geometryEncoderOptions]() {
				if (geometryEncoderOptions.contains(ENC_OPT_OUTPUT_PATH)) {
					return std::filesystem::path(geometryEncoderOptions[ENC_OPT_OUTPUT_PATH].cast<std::string>());
				}
				else {
					const auto fallbackOutputPath = std::filesystem::temp_directory_path() / "pyprt_fallback_output";
					std::filesystem::create_directory(fallbackOutputPath);
					LOG_WRN << "Encoder option '" << ENC_OPT_OUTPUT_PATH
					        << "' was not specified, falling back to system tmp directory:" << fallbackOutputPath;
					return fallbackOutputPath;
				}
			}();
			LOG_DBG << "got outputPath = " << outputPath;

			pcu::FileOutputCallbacksPtr foc;
			if (std::filesystem::is_directory(outputPath) && std::filesystem::exists(outputPath)) {
				foc.reset(prt::FileOutputCallbacks::create(outputPath.wstring().c_str()));
			}
			else {
				LOG_ERR << "The directory specified by '" << ENC_OPT_OUTPUT_PATH
				        << "' is not valid or does not exist: " << outputPath << std::endl;
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
