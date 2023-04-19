// Copyright (c) 2022 Huawei Technologies Co., Ltd
// Copyright (c) 2019, Facebook CORPORATION.
// All rights reserved.
//
// Licensed under the BSD 3-Clause License  (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "torch_npu/csrc/utils/TensorMethods.h"

namespace torch_npu {
namespace utils {

const char* _backend_to_string_npu(const at::Backend& backend) {
  switch (backend) {
    case at::Backend::CPU: return "torch";
    case at_npu::key::NativeBackend: return "torch.npu";
    default: AT_ERROR("Unimplemented backend ", backend);
  }
}

}
}
