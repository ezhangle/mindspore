# Copyright 2020 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
""" tests for quant """
import numpy as np
import pytest

import mindspore.context as context
from mindspore import Tensor
from mindspore import nn
from mindspore.train.quant import quant as qat
from mobilenetv2_combined import MobileNetV2

context.set_context(mode=context.GRAPH_MODE, device_target="GPU")


class LeNet5(nn.Cell):
    """
    Lenet network

    Args:
        num_class (int): Num classes. Default: 10.

    Returns:
        Tensor, output tensor
    Examples:
        >>> LeNet(num_class=10)

    """

    def __init__(self, num_class=10):
        super(LeNet5, self).__init__()
        self.num_class = num_class
        self.conv1 = nn.Conv2dBnAct(1, 6, kernel_size=5, batchnorm=True, activation='relu6', pad_mode="valid")
        self.conv2 = nn.Conv2dBnAct(6, 16, kernel_size=5, activation='relu', pad_mode="valid")
        self.fc1 = nn.DenseBnAct(16 * 5 * 5, 120, activation='relu')
        self.fc2 = nn.DenseBnAct(120, 84, activation='relu')
        self.fc3 = nn.DenseBnAct(84, self.num_class)
        self.max_pool2d = nn.MaxPool2d(kernel_size=2, stride=2)
        self.flatten = nn.Flatten()

    def construct(self, x):
        x = self.conv1(x)
        x = self.max_pool2d(x)
        x = self.conv2(x)
        x = self.max_pool2d(x)
        x = self.flatten(x)
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        return x


@pytest.mark.skip(reason="no `te.lang.cce` in ut env")
def test_qat_lenet():
    img = Tensor(np.ones((32, 1, 32, 32)).astype(np.float32))
    net = LeNet5()
    net = qat.convert_quant_network(
        net, quant_delay=0, bn_fold=False, freeze_bn=10000, weight_bits=8, act_bits=8)
    # should load the checkpoint. mock here
    for param in net.get_parameters():
        param.init_data()
    qat.export_geir(net, img, file_name="quant.pb")


@pytest.mark.skip(reason="no `te.lang.cce` in ut env")
def test_qat_mobile():
    net = MobileNetV2()
    img = Tensor(np.ones((1, 3, 224, 224)).astype(np.float32))
    net = qat.convert_quant_network(
        net, quant_delay=0, bn_fold=True, freeze_bn=10000, weight_bits=8, act_bits=8)
    # should load the checkpoint. mock here
    for param in net.get_parameters():
        param.init_data()
    qat.export_geir(net, img, file_name="quant.pb")
