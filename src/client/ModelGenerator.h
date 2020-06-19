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

#pragma once

#include "InitialShape.h"
#include "GeneratedModel.h"
#include "utils.h"

#include <pybind11/pybind11.h>

#include <filesystem>
#include <vector>

class ModelGenerator {
public:
	ModelGenerator(const std::vector<InitialShape>& myGeo);
	~ModelGenerator() = default;

	std::vector<GeneratedModel> generateModel(const std::vector<pybind11::dict>& shapeAttributes,
	                                          const std::filesystem::path& rulePackagePath,
	                                          const std::wstring& geometryEncoderName,
	                                          const pybind11::dict& geometryEcoderOptions);
	std::vector<GeneratedModel> generateAnotherModel(const std::vector<pybind11::dict>& shapeAttributes);

private:
	pcu::ResolveMapPtr mResolveMap;
	pcu::CachePtr mCache;

	pcu::AttributeMapBuilderPtr mEncoderBuilder;
	std::vector<pcu::AttributeMapPtr> mEncodersOptionsPtr;
	std::vector<std::wstring> mEncodersNames;
	std::vector<pcu::InitialShapeBuilderPtr> mInitialShapesBuilders;

	std::wstring mRuleFile;
	std::wstring mStartRule;
	int32_t mSeed = 0;
	std::wstring mShapeName = L"InitialShape";

	bool mValid = true;

	void setAndCreateInitialShape(const std::vector<pybind11::dict>& shapeAttr,
	                              std::vector<const prt::InitialShape*>& initShapes,
	                              std::vector<pcu::InitialShapePtr>& initShapesPtrs,
	                              std::vector<pcu::AttributeMapPtr>& convertShapeAttr);
	void initializeEncoderData(const std::wstring& encName, const pybind11::dict& encOpt);
};
