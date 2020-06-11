#include "GeneratedModel.h"

GeneratedModel::GeneratedModel(const size_t& initShapeIdx, const std::vector<double>& vert,
                               const std::vector<uint32_t>& indices, const std::vector<uint32_t>& face,
                               const pybind11::dict& rep)
    : mInitialShapeIndex(initShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mReport(rep) {}
