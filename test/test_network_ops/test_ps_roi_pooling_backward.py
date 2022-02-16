# Copyright (c) 2020, Huawei Technologies.All rights reserved.
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
from torch.nn import functional as F

from torch_npu.testing.common_utils import TestCase, run_tests
from torch_npu.testing.common_device_type import Dtypes, instantiate_device_type_tests
from torch_npu.testing.util_test import create_common_tensor, test_2args_broadcast, create_dtype_tensor, UT_FAST_MODE

class TestPsRoiPoolingBackward(TestCase):
    def test_ps_roi_pooling_backward_fp16(self, device):
        roi = torch.tensor([[[1], [2], [3], [4], [5]],
                            [[6], [7], [8], [9], [10]]
                           ], dtype = torch.float16).npu()
        _input = torch.tensor([[[[1]], [[2]], [[3]], [[4]],
                                [[5]], [[6]], [[7]], [[8]]],
                               [[[9]], [[10]], [[11]], [[12]],
                                [[13]], [[14]], [[15]], [[16]]]
                              ], dtype = torch.float16).npu()
        _input.requires_grad = True
        out = torch_npu.npu_ps_roi_pooling(_input, roi, 0.5, 2, 2)
        out.backward(torch.ones_like(out))
        gradout = _input.grad
        expect_gradout = torch.tensor([[[[0.]], [[0.]], [[0.]], [[0.]],
                                        [[0.]], [[0.]], [[0.]], [[0.]]],
                                       [[[0.]], [[0.]], [[0.]], [[0.]],
                                        [[0.]], [[0.]], [[0.]], [[0.]]]
                                      ], dtype = torch.float16)
        expect_out = torch.tensor([[[[0., 0.], [0., 0.]],
                                    [[0., 0.], [0., 0.]]],
                                   [[[0., 0.], [0., 0.]],
                                    [[0., 0.], [0., 0.]]]
                                  ], dtype = torch.float16)

        self.assertRtolEqual(expect_out, out.detach().cpu())
        self.assertRtolEqual(expect_gradout, gradout.cpu())

instantiate_device_type_tests(TestPsRoiPoolingBackward, globals(), except_for="cpu")
if __name__ == "__main__":
    run_tests()