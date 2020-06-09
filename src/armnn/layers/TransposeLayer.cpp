//
// Copyright © 2020 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "TransposeLayer.hpp"

#include "LayerCloneBase.hpp"

#include <armnn/TypesUtils.hpp>

#include <armnnUtils/Transpose.hpp>

#include <backendsCommon/WorkloadData.hpp>
#include <backendsCommon/WorkloadFactory.hpp>

namespace armnn
{

TransposeLayer::TransposeLayer(const TransposeDescriptor& param, const char* name)
    : LayerWithParameters(1, 1, LayerType::Transpose, param, name)
{
}

std::unique_ptr<IWorkload> TransposeLayer::CreateWorkload(const IWorkloadFactory& factory) const
{
    TransposeQueueDescriptor descriptor;
    return factory.CreateTranspose(descriptor, PrepInfoAndDesc(descriptor));
}

TransposeLayer* TransposeLayer::Clone(Graph& graph) const
{
    return CloneBase<TransposeLayer>(graph, m_Param, GetName());
}

std::vector<TensorShape> TransposeLayer::InferOutputShapes(const std::vector<TensorShape>& inputShapes) const
{
    ARMNN_ASSERT(inputShapes.size() == 1);
    const TensorShape& inShape = inputShapes[0];
    return std::vector<TensorShape> ({armnnUtils::TransposeTensorShape(inShape, m_Param.m_DimMappings)});
}

void TransposeLayer::ValidateTensorShapesFromInputs(ShapeInferenceMethod shapeInferenceMethod)
{
    IgnoreUnused(shapeInferenceMethod);

    VerifyLayerConnections(1, CHECK_LOCATION());

    auto inferredShapes = InferOutputShapes({ GetInputSlot(0).GetConnection()->GetTensorInfo().GetShape() });

    ARMNN_ASSERT(inferredShapes.size() == 1);

    ConditionalThrowIfNotEqual<LayerValidationException>(
        "TransposeLayer: TensorShape set on OutputSlot[0] does not match the inferred shape.",
        GetOutputSlot(0).GetTensorInfo().GetShape(),
        inferredShapes[0]);
}

void TransposeLayer::Accept(ILayerVisitor& visitor) const
{
    visitor.VisitTransposeLayer(this, GetParameters(), GetName());
}

} // namespace armnn
