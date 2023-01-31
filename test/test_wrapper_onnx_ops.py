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

import sys
import os

import torch

from torch_npu.testing.testcase import TestCase, run_tests
from torch_npu.testing.common_utils import create_common_tensor
import torch_npu.onnx


class TestOnnxOps(TestCase):

    @classmethod
    def tearDown(cls):
        file_path = sys.path[0]
        file_name = os.listdir()
        for file in file_name:
            if file.endswith('.onnx'):
                os.remove(os.path.join(file_path, file))

    def onnx_export(self, model, x, onnx_model_name, input_names="input_names",
                    output_names="output_names"):

        model.eval()
        OPERATOR_EXPORT_TYPE = torch._C._onnx.OperatorExportTypes.ONNX
        with torch.no_grad():
            torch.onnx.export(model, x,
                              onnx_model_name,
                              opset_version=11,
                              operator_export_type=OPERATOR_EXPORT_TYPE,
                              input_names=["input_names"],
                              output_names=["output_names"])

    def test_wrapper_npu_transpose(self):
        class Model(torch.nn.Module):
            def __init__(self):
                super().__init__()

            def forward(self, x):
                x = torch_npu.npu_transpose(x, (2, 0, 1))
                return x

        def export_onnx():
            x = torch.randn(2, 3, 5).npu()
            model = Model().to("npu")
            self.onnx_export(model, x, "model_npu_transpose.onnx")

        export_onnx()
        assert (os.path.isfile("./model_npu_transpose.onnx"))

    def test_wrapper_npu_broadcast(self):
        class Model(torch.nn.Module):
            def __init__(self):
                super().__init__()

            def forward(self, x):
                x = torch_npu.npu_broadcast(x, (3, 4))
                return x

        def export_onnx():
            x = torch.tensor([[1], [2], [3]]).npu()
            model = Model().to("npu")
            self.onnx_export(model, x, "model_npu_broadcast.onnx")

        export_onnx()
        assert (os.path.isfile("./model_npu_broadcast.onnx"))

    def test_wrapper_npu_slice(self):
        class Model(torch.nn.Module):
            def __init__(self):
                super().__init__()

            def forward(self, x):
                x = torch_npu.npu_slice(x, [0, 0], [2, 2])
                return x

        def export_onnx():
            x = torch.tensor([[1, 2, 3, 4, 5], [6, 7, 8, 9, 10]],
                             dtype=torch.float16).npu()
            model = Model().to("npu")
            self.onnx_export(model, x, "model_npu_slice.onnx")

        export_onnx()
        assert (os.path.isfile("./model_npu_slice.onnx"))

    def test_wrapper_npu_one_hot(self):
        class Model(torch.nn.Module):
            def __init__(self):
                super().__init__()

            def forward(self, x):
                x = torch_npu.npu_one_hot(x, depth=5)
                return x

        def export_onnx():
            x = torch.IntTensor([5, 3, 2, 1]).npu()
            model = Model().to("npu")
            self.onnx_export(model, x, "model_npu_one_hot.onnx")

        export_onnx()
        assert (os.path.isfile("./model_npu_one_hot.onnx"))


if __name__ == '__main__':
    run_tests()
