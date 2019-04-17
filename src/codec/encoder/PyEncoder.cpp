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
const wchar_t*     WNL               = L"\n";

const prtx::EncodePreparator::PreparationFlags ENC_PREP_FLAGS = prtx::EncodePreparator::PreparationFlags()
	.instancing(false)
	.mergeByMaterial(true)
	.triangulate(false)
	.mergeVertices(true)
	.cleanupVertexNormals(true)
	.cleanupUVs(true)
	.processVertexNormals(prtx::VertexNormalProcessor::SET_ALL_TO_FACE_NORMALS);

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
	try {

        prtx::ReportsAccumulatorPtr reportsAccumulator{ prtx::WriteFirstReportsAccumulator::create() };
        prtx::ReportingStrategyPtr reportsCollector{ prtx::LeafShapeReportingStrategy::create(context, initialShapeIndex, reportsAccumulator) };
		const prtx::LeafIteratorPtr li = prtx::LeafIterator::create(context, initialShapeIndex);
		for (prtx::ShapePtr shape = li->getNext(); shape.get() != nullptr; shape = li->getNext()) {
            prtx::ReportsPtr r = reportsCollector->getReports(shape->getID());
			mEncodePreparator->add(context.getCache(), shape, is->getAttributeMap(), r);
		}
	} catch(...) {
		mEncodePreparator->add(context.getCache(), *is, initialShapeIndex);
	}
}


/**
 * After all shapes have been generated, we call the callback function while looping over the
 * finalized geometry instances.
 */
void PyEncoder::finish(prtx::GenerateContext& /*context*/) {
    auto* cb = dynamic_cast<IPyCallbacks*>(getCallbacks());
    if (cb == nullptr)
        throw prtx::StatusException(prt::STATUS_ILLEGAL_CALLBACK_OBJECT);
	const std::wstring baseName = getOptions()->getString(EO_BASE_NAME);

	std::vector<prtx::EncodePreparator::FinalizedInstance> finalizedInstances;
	mEncodePreparator->fetchFinalizedInstances(finalizedInstances, ENC_PREP_FLAGS);

    std::vector<int32_t> shapeIDs;
    shapeIDs.reserve(finalizedInstances.size());

    for (const auto& instance : finalizedInstances) {

        shapeIDs.push_back(instance.getShapeId());

        const prtx::ReportsPtr& rep = instance.getReports();
        
        std::map<std::string, float> reportFloat;
        std::map<std::string, std::string> reportString;
        std::map<std::string, bool> reportBool;
        if (rep) {
            for (prtx::Shape::ReportBool repLine : rep->mBools) {
                std::wstring repName = *(std::get<0>(repLine));
                std::string s(repName.begin(), repName.end());
                reportBool[s] = std::get<1>(repLine);
            }
            for (prtx::Shape::ReportFloat repLine : rep->mFloats) {
                std::wstring repName = *(std::get<0>(repLine));
                std::string s(repName.begin(), repName.end());
                reportFloat[s] = std::get<1>(repLine);
            }
            for (prtx::Shape::ReportString repLine : rep->mStrings) {
                std::wstring repName = *(std::get<0>(repLine));
                std::string s(repName.begin(), repName.end());
                std::wstring repVal = *(std::get<1>(repLine));
                std::string s2(repVal.begin(), repName.end());
                reportString[s] = s2;
            }
        }

        for (const prtx::MeshPtr& m : instance.getGeometry()->getMeshes()) {
            
            const prtx::DoubleVector& vc = m->getVertexCoords();
            std::vector<std::vector<double>> coordMatrix;

            for (int indI = 0; indI < vc.size() / 3; indI++)
            {
                std::vector<double> vertexCoord;
                for (int indJ = 0; indJ < 3; indJ++)
                {
                    vertexCoord.push_back(vc[3 * indI + indJ]);
                }
                coordMatrix.push_back(vertexCoord);
            }

            cb->add(baseName.c_str(), coordMatrix, instance.getShapeId(), reportFloat, reportString, reportBool);

        }
    }
    
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
