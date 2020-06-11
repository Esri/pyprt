#pragma once

#include <cstdint>
#include <string>
#include <vector>

class InitialShape {
public:
	InitialShape(const std::vector<double>& vert);
	InitialShape(const std::vector<double>& vert, const std::vector<uint32_t>& ind,
	             const std::vector<uint32_t>& faceCnt);
	InitialShape(const std::string& path);
	~InitialShape() = default;

	const double* getVertices() const {
		return mVertices.data();
	}
	size_t getVertexCount() const {
		return mVertices.size();
	}
	const uint32_t* getIndices() const {
		return mIndices.data();
	}
	size_t getIndexCount() const {
		return mIndices.size();
	}
	const uint32_t* getFaceCounts() const {
		return mFaceCounts.data();
	}
	size_t getFaceCountsCount() const {
		return mFaceCounts.size();
	}
	const std::string& getPath() const {
		return mPath;
	}
	bool getPathFlag() const {
		return mPathFlag;
	}

protected:
	const std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaceCounts;
	const std::string mPath;
	const bool mPathFlag;
};
