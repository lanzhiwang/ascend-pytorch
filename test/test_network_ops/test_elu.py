# Copyright (c) 2020 Huawei Technologies Co., Ltd
# All rights reserved.
#
# Licensed under the BSD 3-Clause License  (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://opensource.org/licenses/BSD-3-Clause
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import torch
import torch_npu
import numpy as np

from torch_npu.testing.testcase import TestCase, run_tests
from torch_npu.testing.common_utils import create_common_tensor


class TestElu(TestCase):
    def get_format1(self):
        shape_format = [
            [[np.float16, 0, (65535, 1, 1, 1)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 8192)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 16384)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 32768)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 65535)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 131072)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 196608)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 262144)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 393216)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 524288)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 655360)], -2, 2],
            [[np.float16, 0, (1, 1, 1, 786432)], -2, 2],
        ]        
        return shape_format

    def get_format2(self):
        shape_format = [
            [[np.float32, 0, (1, 31, 149, 2)], -1.1754943508e-38, -1.1754943508e-38],
            [[np.float32, 0, (1, 32, 31, 1)], -3402823500.0, 3402823500.0],
            [[np.float32, 0, (128,)], 3402823500, 3402800000],
            [[np.float32, 0, (184965, 1)], -9.313225746154785e-10, 9.313225746154785e-10],
            [[np.float32, 0, (1, 31, 149, 2)], -3402823500.0, -3402823500.0],
            [[np.float32, 0, (1, 31, 149, 2)], -3402823500.0, 3402823500.0],
            [[np.float32, 0, (1, 31, 149, 2)], -9.313225746154785e-10, 9.313225746154785e-10],
            [[np.float32, 0, (2, 31, 149, 2)], -0.000000000000000000000000000000000000011754943508,
            0.000000000000000000000000000000000000011754943508],
            [[np.float32, 0, (4, 31, 149, 2)], 0.000000000000000000000000000000000000011754943508,
            0.000000000000000000000000000000000000011754943508],
            [[np.float32, 0, (2048, 31, 1, 2)], -0.000000000000000000000000000000000000011754943508,
            -0.000000000000000000000000000000000000011754943508],
            [[np.float32, 0, (8, 7, 149)], -0.000000000000000000000000000000000000011754943508,
            0.000000000000000000000000000000000000011754943508]
        ]     
        return shape_format

    def cpu_op_exec(self, input1):
        output = torch.nn.functional.elu(input1)
        output = output.numpy()
        return output

    def cpu_op_exec_fp16(self, input1):
        input1 = input1.to(torch.float32)
        output = torch.nn.functional.elu(input1)
        output = output.to(torch.float16)
        output = output.numpy()
        return output

    def npu_op_exec(self, input1):
        output = torch.nn.functional.elu(input1)
        output = output.to("cpu")
        output = output.numpy()
        return output

    def test_elu_common_shape_format_fp16(self):
        shape_format = self.get_format1()
        for item in shape_format:
            cpu_input1, npu_input1 = create_common_tensor(item[0], item[1], item[2])
            cpu_output = self.cpu_op_exec_fp16(cpu_input1)
            npu_output = self.npu_op_exec(npu_input1)
            self.assertRtolEqual(cpu_output, npu_output)

    def test_elu_common_shape_format_fp32(self):
        shape_format = self.get_format2()
        for item in shape_format:
            cpu_input1, npu_input1 = create_common_tensor(item[0], item[1], item[2])
            cpu_output = self.cpu_op_exec(cpu_input1)
            npu_output = self.npu_op_exec(npu_input1)
            self.assertRtolEqual(cpu_output, npu_output)

if __name__ == '__main__':
    run_tests()
