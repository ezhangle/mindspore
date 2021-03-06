/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP16_DECONVOLUTION_DEPTHWISE_FP16_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP16_DECONVOLUTION_DEPTHWISE_FP16_H_

#include <vector>
#include "src/lite_kernel.h"
#include "src/runtime/kernel/arm/base/convolution_base.h"
#include "src/runtime/kernel/arm/nnacl/fp16/conv_depthwise_fp16.h"

namespace mindspore::kernel {
class DeconvolutionDepthwiseFp16CPUKernel : public ConvolutionBaseCPUKernel {
 public:
  DeconvolutionDepthwiseFp16CPUKernel(OpParameter *parameter, const std::vector<lite::tensor::Tensor *> &inputs,
                                      const std::vector<lite::tensor::Tensor *> &outputs, const Context *ctx,
                                      const lite::Primitive *primitive)
      : ConvolutionBaseCPUKernel(parameter, inputs, outputs, ctx, primitive) {}
  ~DeconvolutionDepthwiseFp16CPUKernel() override {
    delete sliding_;
    free(packed_weight_);
    if (need_align_) {
      free(packed_input_);
      free(packed_output_);
    }
  };

  int Init() override;
  int ReSize() override;
  int Run() override;

  int InitBuffer();
  int InitWeightBias();
  int InitSlideParam();
  int Execute(int task_id);

 private:
  SlidingWindowParam *sliding_;
  float16_t *packed_weight_;
  float16_t *packed_input_;
  float16_t *packed_output_;
  bool need_align_ = false;
};
}  // namespace mindspore::kernel

#endif  // MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_FP16_DECONVOLUTION_DEPTHWISE_FP16_H_
