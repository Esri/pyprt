/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2024 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
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
#include "PyCallbacks.h"
#include "logging.h"

#include <memory>

namespace {

const std::wstring ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const std::wstring ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
const std::wstring ENCODER_ID_CGA_ERROR = L"com.esri.prt.core.CGAErrorEncoder";
const std::wstring ENCODER_ID_ATTR_EVAL = L"com.esri.prt.core.AttributeEvalEncoder";
const std::wstring ENCODER_ID_PYTHON = L"com.esri.pyprt.PyEncoder";

constexpr const char* ENC_OPT_OUTPUT_PATH = "outputPath";

void extractMainShapeAttributes(const py::dict& shapeAttr, int32_t& seed, std::wstring& shapeName,
                                AttributeMapPtr& convertShapeAttr) {
	convertShapeAttr = pcu::createAttributeMapFromPythonDict(
	        shapeAttr, *(AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));
	if (convertShapeAttr) {
		if (convertShapeAttr->hasKey(L"seed") && convertShapeAttr->getType(L"seed") == prt::AttributeMap::PT_INT)
			seed = convertShapeAttr->getInt(L"seed");
		if (convertShapeAttr->hasKey(L"shapeName") &&
		    convertShapeAttr->getType(L"shapeName") == prt::AttributeMap::PT_STRING)
			shapeName = convertShapeAttr->getString(L"shapeName");
	}
}

// Chicken and egg situation for initial shapes from assets:
// PRT requires to have any dependencies of the asset (e.g. textures) in the resolve map.
// As we did not yet decode the asset, we do not know them.
// Heuristic: We assume all dependencies to be located in the same file system and
// directory tree rooted at the containing directory of the asset.
ResolveMapPtr createResolveMapForInitialShape(const std::filesystem::path& assetPath, const uint8_t recursionLimit) {
	if (!assetPath.is_absolute() || !std::filesystem::is_regular_file(assetPath))
		return {};

	ResolveMapBuilderPtr rmb(prt::ResolveMapBuilder::create());

	// add main asset file
	const std::wstring assetUri = pcu::toUTF16FromUTF8(pcu::toFileURI(assetPath.generic_string()));
	rmb->addEntry(assetPath.generic_wstring().c_str(), assetUri.c_str());

	for (auto it = std::filesystem::recursive_directory_iterator(assetPath.parent_path());
	     it != std::filesystem::recursive_directory_iterator(); ++it) {
		if (it.depth() > recursionLimit) {
			it.disable_recursion_pending();
			continue;
		}
		LOG_DBG << "d = " << it.depth() << ": " << it->path();
		if (std::filesystem::is_regular_file(it->path())) {
			const std::wstring uri = pcu::toUTF16FromUTF8(pcu::toFileURI(it->path().generic_string()));
			rmb->addEntry(it->path().generic_wstring().c_str(), uri.c_str());
		}
	}

	ResolveMapPtr resolveMap(rmb->createResolveMap());
	return resolveMap;
}

} // namespace

ModelGenerator::ModelGenerator(const std::vector<InitialShape>& protoShapes)
    : mCache(prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT)) {
	mInitialShapesBuilders.reserve(protoShapes.size());
	for (const InitialShape& protoShape : protoShapes) {
		InitialShapeBuilderPtr isb{prt::InitialShapeBuilder::create()};

		if (protoShape.initializedFromPath()) {
			if (!pcu::toFileURI(protoShape.getPath()).empty()) {
				LOG_DBG << "trying to read initial shape geometry from " << protoShape.getPath();
				const std::filesystem::path assetPath = std::filesystem::path(protoShape.getPath());

				// create temporary resolve map for initial shape builder to scan for embedded resources
				ResolveMapPtr resolveMap =
				        createResolveMapForInitialShape(assetPath, protoShape.getDirectoryRecursionDepth());
				if (!resolveMap) {
					LOG_WRN << "could not scan asset path for related files (e.g. textures) - the initial shape asset "
					           "might not be complete."
					        << assetPath;
					// we keep the ModelGenerator valid in this case, PRT will emit an e.g. "texture not found" warning
				}
				else {
					LOG_DBG << "resolve map for embedded resources in asset " << assetPath << ":\n"
					        << pcu::objectToXML(resolveMap.get()) << std::endl;
				}

				const prt::Status s =
				        isb->resolveGeometry(assetPath.generic_wstring().c_str(), resolveMap.get(), mCache.get());
				if (s != prt::STATUS_OK) {
					LOG_ERR << "could not resolve geometry from " << pcu::toFileURI(protoShape.getPath());
					mValid = false;
				}
			}
			else {
				LOG_ERR << "could not read initial shape geometry, invalid path";
				mValid = false;
			}
		}
		else {
			const prt::Status status = isb->setGeometry(protoShape.getVertices(), protoShape.getVertexCount(),
			                                            protoShape.getIndices(), protoShape.getIndexCount(),
			                                            protoShape.getFaceCounts(), protoShape.getFaceCountsCount(),
			                                            protoShape.getHoles(), protoShape.getHolesCount());
			mValid = (status == prt::STATUS_OK);
			if (!mValid)
				LOG_ERR << "Failed to initialize ModelGenerator: invalid input geometry";
		}

		mInitialShapesBuilders.emplace_back(mValid ? std::move(isb) : InitialShapeBuilderPtr());
	}
}

void ModelGenerator::setAndCreateInitialShape(const std::vector<py::dict>& shapesAttr,
                                              std::vector<const prt::InitialShape*>& initShapes,
                                              std::vector<InitialShapePtr>& initShapePtrs,
                                              std::vector<AttributeMapPtr>& convertedShapeAttr) {
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
	const AttributeMapPtr encOptions{pcu::createAttributeMapFromPythonDict(encOpt, *mEncoderBuilder)};
	mEncodersOptionsPtr.push_back(pcu::createValidatedOptions(encName.c_str(), encOptions));

	mEncodersNames.push_back(ENCODER_ID_CGA_REPORT);
	mEncodersNames.push_back(ENCODER_ID_CGA_PRINT);
	mEncodersNames.push_back(ENCODER_ID_CGA_ERROR);
	mEncodersNames.push_back(ENCODER_ID_ATTR_EVAL);

	const AttributeMapBuilderPtr optionsBuilder{prt::AttributeMapBuilder::create()};
	const AttributeMapPtr reportOptions{optionsBuilder->createAttributeMapAndReset()};
	const AttributeMapPtr printOptions{optionsBuilder->createAttributeMapAndReset()};
	const AttributeMapPtr errorOptions{optionsBuilder->createAttributeMapAndReset()};
	const AttributeMapPtr attrOptions{optionsBuilder->createAttributeMapAndReset()};

	mEncodersOptionsPtr.push_back(pcu::createValidatedOptions(ENCODER_ID_CGA_REPORT, reportOptions));
	mEncodersOptionsPtr.push_back(pcu::createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions));
	mEncodersOptionsPtr.push_back(pcu::createValidatedOptions(ENCODER_ID_CGA_ERROR, errorOptions));
	mEncodersOptionsPtr.push_back(pcu::createValidatedOptions(ENCODER_ID_ATTR_EVAL, attrOptions));
}

prt::Status ModelGenerator::initializeRulePackageData(const std::filesystem::path& rulePackagePath,
                                                      ResolveMapPtr& resolveMap, CachePtr& cache) {
	if (!std::filesystem::exists(rulePackagePath)) {
		LOG_ERR << "The rule package path is unvalid.";
		return prt::STATUS_FILE_NOT_FOUND;
	}

	if (!pcu::getResolveMap(rulePackagePath, &resolveMap))
		return prt::STATUS_RESOLVEMAP_PROVIDER_NOT_FOUND;

	mRuleFile = pcu::getRuleFileEntry(resolveMap.get());

	const wchar_t* ruleFileURI = resolveMap->getString(mRuleFile.c_str());
	if (ruleFileURI == nullptr) {
		LOG_ERR << "could not find rule file URI in resolve map of rule package " << rulePackagePath;
		return prt::STATUS_INVALID_URI;
	}

	prt::Status infoStatus = prt::STATUS_UNSPECIFIED_ERROR;
	RuleFileInfoUPtr info(prt::createRuleFileInfo(ruleFileURI, cache.get(), &infoStatus));
	if (!info || infoStatus != prt::STATUS_OK) {
		LOG_ERR << "could not get rule file info from rule file " << mRuleFile;
		return infoStatus;
	}

	mStartRule = pcu::detectStartRule(info);
	mHiddenAttrs = pcu::getHiddenAttributes(info);
	return prt::STATUS_OK;
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

	try {
		// Rule package
		prt::Status rpkStat = initializeRulePackageData(rulePackagePath, mResolveMap, mCache);

		if (rpkStat != prt::STATUS_OK)
			return {};

		// Initial shapes
		std::vector<const prt::InitialShape*> initialShapes(mInitialShapesBuilders.size());
		std::vector<InitialShapePtr> initialShapePtrs(mInitialShapesBuilders.size());
		std::vector<AttributeMapPtr> convertedShapeAttrVec(mInitialShapesBuilders.size());
		setAndCreateInitialShape(shapeAttributes, initialShapes, initialShapePtrs, convertedShapeAttrVec);

		// Encoder info, encoder options
		if (!mEncoderBuilder)
			mEncoderBuilder.reset(prt::AttributeMapBuilder::create());

		initializeEncoderData(geometryEncoderName, geometryEncoderOptions);

		assert(mEncodersNames.size() == mEncodersOptionsPtr.size());
		const std::vector<const wchar_t*> encoders = pcu::toPtrVec(mEncodersNames);
		const std::vector<const prt::AttributeMap*> encodersOptions = pcu::toPtrVec(mEncodersOptionsPtr);
		assert(encoders.size() == encodersOptions.size());

		if (geometryEncoderName == ENCODER_ID_PYTHON) {

			PyCallbacksPtr foc{std::make_unique<PyCallbacks>(mInitialShapesBuilders.size(), mHiddenAttrs)};

			// Generate
			const prt::Status genStat =
			        prt::generate(initialShapes.data(), initialShapes.size(), nullptr, encoders.data(), encoders.size(),
			                      encodersOptions.data(), foc.get(), mCache.get(), nullptr);

			if (genStat != prt::STATUS_OK) {
				LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' ("
				        << genStat << ")";
				return {};
			}

			std::vector<GeneratedModel> newGeneratedGeo;
			newGeneratedGeo.reserve(mInitialShapesBuilders.size());
			for (size_t idx = 0; idx < mInitialShapesBuilders.size(); idx++) {
				newGeneratedGeo.emplace_back(idx, foc->getGeneratedPayload(idx));
			}
			return newGeneratedGeo;
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

			FileOutputCallbacksPtr foc;
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
	}
	catch (...) {
		LOG_ERR << "caught unknown exception.";
	}

	return {};
}