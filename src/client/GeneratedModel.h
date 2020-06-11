#pragma once

#include <pybind11/pybind11.h>

#include <cstddef>
#include <cstdint>
#include <vector>

class GeneratedModel {
public:
	GeneratedModel(const size_t& initialShapeIdx, const std::vector<double>& vert, const std::vector<uint32_t>& indices,
	               const std::vector<uint32_t>& face, const pybind11::dict& rep);
	GeneratedModel() = default;
	~GeneratedModel() = default;

	size_t getInitialShapeIndex() const {
		return mInitialShapeIndex;
	}
	const std::vector<double>& getVertices() const {
		return mVertices;
	}
	const std::vector<uint32_t>& getIndices() const {
		return mIndices;
	}
	const std::vector<uint32_t>& getFaces() const {
		return mFaces;
	}
	const pybind11::dict& getReport() const {
		return mReport;
	}

private:
	size_t mInitialShapeIndex;
	std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaces;
	pybind11::dict mReport;
};

PYBIND11_MAKE_OPAQUE(std::vector<GeneratedModel>);
