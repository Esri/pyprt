/**
 * Esri CityEngine SDK Geometry Encoder for Python
 *
 * This example demonstrates the usage of the PRTX interface
 * to write custom encoders.
 *
 * See README.md in http://github.com/ArcGIS/esri-cityengine-sdk for build instructions.
 *
 * Written by Simon Haegler
 * Modified by Camille Lechot
 * Esri R&D Center Zurich, Switzerland
 *
 * Copyright 2012-2017 (c) Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "PyEncoder.h"
#include "IPyCallbacks.h"

#include "prtx/Shape.h"
#include "prtx/ShapeIterator.h"
#include "prtx/GenerateContext.h"
#include "prtx/Geometry.h"
#include "prtx/Mesh.h"
#include "prtx/EncodeOptions.h"
#include "prtx/EncoderInfoBuilder.h"
#include "prtx/ReportsCollector.h"
#include "prtx/Exception.h"
#include "prtx/prtx.h"
#include "prtx/Attributable.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>


namespace {

const wchar_t*     EO_BASE_NAME      = L"baseName";
const wchar_t*     EO_ERROR_FALLBACK = L"errorFallback";
const std::wstring ENCFILE_EXT       = L".txt";
const wchar_t*     EO_EMIT_REPORTS = L"emitReports";
const wchar_t*     EO_EMIT_GEOMETRY = L"emitGeometry";


const prtx::EncodePreparator::PreparationFlags ENC_PREP_FLAGS = prtx::EncodePreparator::PreparationFlags()
    .instancing(false)
    .triangulate(false)
    .mergeVertices(false)
    .cleanupUVs(false)
    .cleanupVertexNormals(false)
    .mergeByMaterial(true); // if false, generation takes ages... 40 sec instead of 1.5 sec
} // namespace


const std::wstring PyEncoder::ID          = L"com.esri.prt.examples.PyEncoder";
const std::wstring PyEncoder::NAME        = L"Python Geometry Encoder";
const std::wstring PyEncoder::DESCRIPTION = L"Encodes geometry for Python.";


/**
 * Setup two namespaces for mesh and material objects and initialize the encode preprator.
 * The namespaces are used to create unique names for all mesh and material objects.
 */
void PyEncoder::init(prtx::GenerateContext& /*context*/) {
	prtx::NamePreparator::NamespacePtr nsMaterials = mNamePreparator.newNamespace();
	prtx::NamePreparator::NamespacePtr nsMeshes = mNamePreparator.newNamespace();
    mEncodePreparator = prtx::EncodePreparator::create(true, mNamePreparator, nsMeshes, nsMaterials);
}


/**
 * During encoding we collect the resulting shapes with the encode preparator.
 * In case the shape generation fails, we collect the initial shape.
 */
void PyEncoder::encode(prtx::GenerateContext& context, size_t initialShapeIndex) {

	const prtx::InitialShape* is = context.getInitialShape(initialShapeIndex);
    auto* cb = dynamic_cast<IPyCallbacks*>(getCallbacks());

    if (getOptions()->getBool(EO_EMIT_REPORTS)) {
        prtx::ReportsAccumulatorPtr reportsAccumulator{ prtx::SummarizingReportsAccumulator::create() };
        prtx::ReportingStrategyPtr reportsCollector{ prtx::AllShapesReportingStrategy::create(context, initialShapeIndex, reportsAccumulator) };

        prtx::ReportsPtr rep = reportsCollector->getReports();
        FloatMap reportFloat;
        StringMap reportString;
        BoolMap reportBool;

        if (rep) {
            prtx::Shape::ReportBoolVect& boolReps = rep->mBools;
            for (size_t i = 0; i < boolReps.size(); i++) {
                const wchar_t* repName = boolReps[i].first->c_str();
                const bool repVal = boolReps[i].second;
                std::wstring repNameWString = repName;
                std::string s(repNameWString.begin(), repNameWString.end());
                reportBool[s] = repVal;
            }

            prtx::Shape::ReportFloatVect& floatReps = rep->mFloats;
            for (size_t i = 0; i < floatReps.size(); i++) {
                const wchar_t* repName = floatReps[i].first->c_str();
                const double repVal = floatReps[i].second;
                std::wstring repNameWString = repName;
                std::string s(repNameWString.begin(), repNameWString.end());
                reportFloat[s] = repVal;
            }

            prtx::Shape::ReportStringVect&	stringReps = rep->mStrings;
            for (size_t i = 0; i < stringReps.size(); i++) {
                const wchar_t* repName = stringReps[i].first->c_str();
                const wchar_t* repVal = stringReps[i].second->c_str();
                std::wstring repNameWString = repName;
                std::wstring repValWString = repVal;
                std::string s(repNameWString.begin(), repNameWString.end());
                std::string s2(repValWString.begin(), repValWString.end());
                reportString[s] = s2;
            }
        }

        cb->addReports(initialShapeIndex, reportFloat, reportString, reportBool);
    }

    if (getOptions()->getBool(EO_EMIT_GEOMETRY)) {
        try {
            const prtx::LeafIteratorPtr li = prtx::LeafIterator::create(context, initialShapeIndex);

            for (prtx::ShapePtr shape = li->getNext(); shape.get() != nullptr; shape = li->getNext()) {
                mEncodePreparator->add(context.getCache(), shape, is->getAttributeMap());
            }
        }
        catch (...) {
            mEncodePreparator->add(context.getCache(), *is, initialShapeIndex);
        }

        std::vector<prtx::EncodePreparator::FinalizedInstance> finalizedInstances;
        mEncodePreparator->fetchFinalizedInstances(finalizedInstances, ENC_PREP_FLAGS);

        for (const auto& instance : finalizedInstances) {
            std::vector<std::vector<double>> coordMatrix;
            std::vector<std::vector<uint32_t>> faceMatrix;

            for (const prtx::MeshPtr& mesh : instance.getGeometry()->getMeshes()) {

                const prtx::DoubleVector& vc = mesh->getVertexCoords();
                const uint32_t faceCnt = mesh->getFaceCount();

                for (size_t indI = 0; indI < vc.size() / 3; indI++)
                {
                    std::vector<double> vertexCoord;
                    for (int indJ = 0; indJ < 3; indJ++)
                    {
                        vertexCoord.push_back(vc[3 * indI + indJ]);
                    }
                    coordMatrix.push_back(vertexCoord);
                }

                for (uint32_t fi = 0; fi < faceCnt; ++fi) {
                    const uint32_t* vtxIdx = mesh->getFaceVertexIndices(fi);
                    const uint32_t vtxCnt = mesh->getFaceVertexCount(fi);

                    std::vector<uint32_t> v(vtxIdx, vtxIdx + vtxCnt);
                    faceMatrix.push_back(v);
                }

            }

            cb->addGeometry(instance.getInitialShapeIndex(), coordMatrix, faceMatrix);
        }
    }

}


/**
 * After all shapes have been generated, we call the callback function while looping over the
 * finalized geometry instances.
 */
void PyEncoder::finish(prtx::GenerateContext& /*context*/) {
}


/**
 * Create the encoder factory singleton and define the default options.
 */
PyEncoderFactory* PyEncoderFactory::createInstance() {
	prtx::EncoderInfoBuilder encoderInfoBuilder;

	encoderInfoBuilder.setID(PyEncoder::ID);
	encoderInfoBuilder.setName(PyEncoder::NAME);
	encoderInfoBuilder.setDescription(PyEncoder::DESCRIPTION);
	encoderInfoBuilder.setType(prt::CT_GEOMETRY);
	encoderInfoBuilder.setExtension(ENCFILE_EXT);

	// optionally we could setup a validator
	// encoderInfoBuilder.setValidator(prtx::EncodeOptionsValidatorPtr(new MyOptionsValidator()));

	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());
	amb->setString(EO_BASE_NAME, L"enc_default_name"); // required by CityEngine
	amb->setBool(EO_ERROR_FALLBACK, prtx::PRTX_TRUE); // required by CityEngine
    amb->setBool(EO_EMIT_REPORTS, prtx::PRTX_TRUE);
    amb->setBool(EO_EMIT_GEOMETRY, prtx::PRTX_TRUE);
	encoderInfoBuilder.setDefaultOptions(amb->createAttributeMap());

	// CityEngine requires the following annotations to create an UI for an option:
	// label, order, group and description
	prtx::EncodeOptionsAnnotator eoa(encoderInfoBuilder);
	eoa.option(EO_BASE_NAME)
			.setLabel(L"Base Name")
			.setOrder(0.0)
			.setGroup(L"General Settings", 0.0)
			.setDescription(L"Sets the base name of the written file.");

	// Hide the error fallback option in the CityEngine UI.
	eoa.option(EO_ERROR_FALLBACK).flagAsHidden();

	return new PyEncoderFactory(encoderInfoBuilder.create());
}
