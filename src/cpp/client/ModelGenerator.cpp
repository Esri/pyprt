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

#include "pybind11/pybind11.h"

#include <memory>

namespace py = pybind11;

namespace {

const std::wstring ENCODER_ID_CGA_REPORT = L"com.esri.prt.core.CGAReportEncoder";
const std::wstring ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";
const std::wstring ENCODER_ID_CGA_ERROR = L"com.esri.prt.core.CGAErrorEncoder";
const std::wstring ENCODER_ID_ATTR_EVAL = L"com.esri.prt.core.AttributeEvalEncoder";
const std::wstring ENCODER_ID_PYTHON = L"com.esri.pyprt.PyEncoder";

constexpr const char* ENC_OPT_OUTPUT_PATH = "outputPath";

/**
 * Helper function to convert a Python dictionary of "<key>:<value>" into a
 * prt::AttributeMap
 */
AttributeMapPtr createAttributeMapFromPythonDict(const py::dict& args, prt::AttributeMapBuilder& bld) {
	for (auto a : args) {

		const std::wstring key = a.first.cast<std::wstring>();

		if (py::isinstance<py::list>(a.second.ptr())) {
			auto li = a.second.cast<py::list>();

			if (py::isinstance<py::bool_>(li[0])) {
				try {
					size_t count = li.size();
					std::unique_ptr<bool[]> v_arr(new bool[count]);

					for (size_t i = 0; i < count; i++) {
						bool item = li[i].cast<bool>();
						v_arr[i] = item;
					}

					bld.setBoolArray(key.c_str(), v_arr.get(), count);
				}
				catch (std::exception& e) {
					LOG_ERR << L"cannot set bool array attribute " << key << ": " << e.what();
				}
			}
			else if (py::isinstance<py::float_>(li[0])) {
				try {
					const size_t count = li.size();
					std::vector<double> v_arr(count);
					for (size_t i = 0; i < v_arr.size(); i++) {
						double item = li[i].cast<double>();
						v_arr[i] = item;
					}

					bld.setFloatArray(key.c_str(), v_arr.data(), v_arr.size());
				}
				catch (std::exception& e) {
					LOG_ERR << L"cannot set float array attribute " << key << ": " << e.what();
				}
			}
			else if (py::isinstance<py::int_>(li[0])) {
				try {
					const size_t count = li.size();
					std::vector<int32_t> v_arr(count);
					for (size_t i = 0; i < v_arr.size(); i++) {
						int32_t item = li[i].cast<int32_t>();
						v_arr[i] = item;
					}

					bld.setIntArray(key.c_str(), v_arr.data(), v_arr.size());
				}
				catch (std::exception& e) {
					std::wcerr << L"cannot set int array attribute " << key << ": " << e.what() << std::endl;
				}
			}
			else if (py::isinstance<py::str>(li[0])) {
				const size_t count = li.size();
				std::vector<std::wstring> v_arr(count);
				for (size_t i = 0; i < v_arr.size(); i++) {
					std::wstring item = li[i].cast<std::wstring>();
					v_arr[i] = item;
				}

				const auto v_arr_ptrs = pcu::toPtrVec(v_arr); // setStringArray requires contiguous array
				bld.setStringArray(key.c_str(), v_arr_ptrs.data(), v_arr_ptrs.size());
			}
			else
				LOG_WRN << "Encountered unknown array type for key " << key;
		}
		else {
			if (py::isinstance<py::bool_>(a.second.ptr())) { // check for boolean first!!
				try {
					bool val = a.second.cast<bool>();
					bld.setBool(key.c_str(), val);
				}
				catch (std::exception& e) {
					LOG_ERR << "cannot set bool attribute " << key << ": " << e.what();
				}
			}
			else if (py::isinstance<py::float_>(a.second.ptr())) {
				try {
					double val = a.second.cast<double>();
					bld.setFloat(key.c_str(), val);
				}
				catch (std::exception& e) {
					LOG_ERR << "cannot set float attribute " << key << ": " << e.what();
				}
			}
			else if (py::isinstance<py::int_>(a.second.ptr())) {
				try {
					int32_t val = a.second.cast<int32_t>();
					bld.setInt(key.c_str(), val);
				}
				catch (std::exception& e) {
					std::wcerr << L"cannot set int attribute " << key << ": " << e.what() << std::endl;
				}
			}
			else if (py::isinstance<py::str>(a.second.ptr())) {
				std::wstring val = a.second.cast<std::wstring>();
				bld.setString(key.c_str(), val.c_str());
			}
			else
				LOG_WRN << "Encountered unknown scalar type for key " << key;
		}
	}
	return AttributeMapPtr{bld.createAttributeMap()};
}

void extractMainShapeAttributes(const py::dict& shapeAttr, int32_t& seed, std::wstring& shapeName,
                                AttributeMapPtr& convertShapeAttr) {
	convertShapeAttr =
	        createAttributeMapFromPythonDict(shapeAttr, *(AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));
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
	const AttributeMapPtr encOptions{createAttributeMapFromPythonDict(encOpt, *mEncoderBuilder)};
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
                                                      ResolveMapPtr& resolveMap, CachePtr& cache,
                                                      const pybind11::dict& assets) {
	if (!std::filesystem::exists(rulePackagePath)) {
		LOG_ERR << "The rule package path is unvalid.";
		return prt::STATUS_FILE_NOT_FOUND;
	}

	resolveMap = pcu::getResolveMap(rulePackagePath);
	if (!resolveMap)
		return prt::STATUS_RESOLVEMAP_PROVIDER_NOT_FOUND;

	if (!assets.empty()) {
		ResolveMapBuilderPtr rmb(prt::ResolveMapBuilder::createFromResolveMap(resolveMap.get()));
		for (const auto& asset : assets) {
			const auto key = asset.first.cast<std::wstring>();
			const auto uri = asset.second.cast<std::wstring>();
			rmb->addEntry(key.c_str(), uri.c_str());
		}
		resolveMap.reset(rmb->createResolveMap());
	}

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
                                                          const py::dict& geometryEncoderOptions,
                                                          const pybind11::dict& assets) {
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
		prt::Status rpkStat = initializeRulePackageData(rulePackagePath, mResolveMap, mCache, assets);

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