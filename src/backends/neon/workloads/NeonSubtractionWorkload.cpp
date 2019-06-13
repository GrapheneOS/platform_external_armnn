//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "NeonSubtractionWorkload.hpp"

#include "NeonWorkloadUtils.hpp"
#include <aclCommon/ArmComputeTensorUtils.hpp>
#include <backendsCommon/CpuTensorHandle.hpp>

#include <arm_compute/runtime/NEON/functions/NEArithmeticSubtraction.h>

namespace armnn
{

arm_compute::Status NeonSubtractionWorkloadValidate(const TensorInfo& input0,
                                                    const TensorInfo& input1,
                                                    const TensorInfo& output)
{
    const arm_compute::TensorInfo aclInput0 = armcomputetensorutils::BuildArmComputeTensorInfo(input0);
    const arm_compute::TensorInfo aclInput1 = armcomputetensorutils::BuildArmComputeTensorInfo(input1);
    const arm_compute::TensorInfo aclOutput = armcomputetensorutils::BuildArmComputeTensorInfo(output);

    return arm_compute::NEArithmeticSubtraction::validate(&aclInput0,
                                                          &aclInput1,
                                                          &aclOutput,
                                                          arm_compute::ConvertPolicy::SATURATE);
}

NeonSubtractionWorkload::NeonSubtractionWorkload(const SubtractionQueueDescriptor& descriptor,
                                                 const WorkloadInfo& info)
    : BaseWorkload<SubtractionQueueDescriptor>(descriptor, info)
{
    m_Data.ValidateInputsOutputs("NeonSubtractionWorkload", 2, 1);

    arm_compute::ITensor& input1 = boost::polymorphic_downcast<IAclTensorHandle*>(m_Data.m_Inputs[0])->GetTensor();
    arm_compute::ITensor& input2 = boost::polymorphic_downcast<IAclTensorHandle*>(m_Data.m_Inputs[1])->GetTensor();
    arm_compute::ITensor& output = boost::polymorphic_downcast<IAclTensorHandle*>(m_Data.m_Outputs[0])->GetTensor();

    auto layer = std::make_unique<arm_compute::NEArithmeticSubtraction>();
    layer->configure(&input1, &input2, &output, arm_compute::ConvertPolicy::SATURATE);
    m_SubLayer.reset(layer.release());
}

void NeonSubtractionWorkload::Execute() const
{
    ARMNN_SCOPED_PROFILING_EVENT_NEON("NeonSubtractionWorkload_Execute");
    m_SubLayer->run();
}

} //namespace armnn
