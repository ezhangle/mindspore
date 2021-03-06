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

#include <memory>
#include "mindspore/lite/tools/converter/parser/caffe/caffe_eltwise_parser.h"

const int ELTWISE_MIN_INPUT_SIZE = 2;

namespace mindspore {
namespace lite {
STATUS CaffeEltwiseParser::Parse(const caffe::LayerParameter &proto, const caffe::LayerParameter &weight,
                                 schema::CNodeT *op, std::vector<schema::TensorT *> *weightVec) {
  std::unique_ptr<schema::EltwiseT> attr(new schema::EltwiseT());
  if (proto.bottom_size() < ELTWISE_MIN_INPUT_SIZE) {
    // MS_LOGE("Eltwise Op '%s' need at least 2 inputs,but input size is %d",  proto.name().c_str(),
    // proto.bottom_size());
    return RET_ERROR;
  }

  const caffe::EltwiseParameter eltwiseParam = proto.eltwise_param();

  if (eltwiseParam.coeff_size() != 0 && eltwiseParam.coeff_size() != proto.bottom_size()) {
    // MS_LOGE("Coeff size(%d) check fail, Eltwise Layer takes one coefficient per bottom blob.",
    //        eltwiseParam.coeff_size());
    return RET_PARAM_INVALID;
  }

  if (eltwiseParam.operation() == caffe::EltwiseParameter::PROD && eltwiseParam.coeff_size() != 0) {
    // MS_LOGE("Eltwise layer only takes coefficients for summation.");
    return RET_ERROR;
  }

  if (proto.has_eltwise_param() && eltwiseParam.has_operation()) {
    switch (eltwiseParam.operation()) {
      case caffe::EltwiseParameter::PROD:
        attr->mode = schema::EltwiseMode_PROD;
        break;
      case caffe::EltwiseParameter::SUM:
        attr->mode = schema::EltwiseMode_SUM;
        break;
      case caffe::EltwiseParameter::MAX:
        attr->mode = schema::EltwiseMode_MAXIMUM;
        break;
      default:
        // MS_LOGE("Eltwise parse params fail, unsupported opration %d.", eltwiseParam.operation());
        return RET_PARAM_INVALID;
    }
  } else {
    attr->mode = schema::EltwiseMode_SUM;
  }
  op->primitive = std::make_unique<schema::PrimitiveT>();
  op->primitive->value.type = schema::PrimitiveType_Eltwise;
  op->primitive->value.value = attr.release();
  return RET_OK;
}

CaffeNodeRegistrar g_caffeEltwiseParser("Eltwise", new CaffeEltwiseParser());
}  // namespace lite
}  // namespace mindspore
