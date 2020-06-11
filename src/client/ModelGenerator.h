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
	void getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc,
	                               std::vector<const prt::AttributeMap*>& allEncOpt);
};
