#include "InitialShape.h"

#include <numeric>

InitialShape::InitialShape(const std::vector<double>& vert) : mVertices(vert), mPathFlag(false) {
	mIndices.resize(vert.size() / 3);
	std::iota(std::begin(mIndices), std::end(mIndices), 0);
	mFaceCounts.resize(1, (uint32_t)mIndices.size());
}

InitialShape::InitialShape(const std::vector<double>& vert, const std::vector<uint32_t>& ind,
                           const std::vector<uint32_t>& faceCnt)
    : mVertices(vert), mIndices(ind), mFaceCounts(faceCnt), mPathFlag(false) {}

InitialShape::InitialShape(const std::string& initShapePath) : mPath(initShapePath), mPathFlag(true) {}
