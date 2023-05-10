// Copyright (c) 2020 Huawei Technologies Co., Ltd
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

#pragma once

#include "torch_npu/csrc/core/npu/npu_log.h"
#include "torch_npu/csrc/core/npu/sys_ctrl/npu_sys_ctrl.h"
#include "torch_npu/csrc/core/npu/NPUErrorCodes.h"
#include <memory>

#include "hccl/hccl.h"
#include "hccl/hccl_types.h"

#define C10D_HCCL_CHECK(cmd)                                        \
  do {                                                              \
    HcclResult error = cmd;                                         \
    if (error != HCCL_SUCCESS) {                                    \
      std::string err = "HCCL error in: " + std::string(__FILE__) + \
          ":" + std::to_string(__LINE__) + ".\n" +                  \
          " HCCL error, error code is " +                           \
          std::to_string(error) + ". " +                            \
          (c10_npu::acl::hcclresult_code_map.find(error) !=         \
          c10_npu::acl::hcclresult_code_map.end() ?                 \
          " [error]: " +                                            \
          c10_npu::acl::hcclresult_code_map[error] +                \
          "\n" : " " )  +                                           \
          "And see details in Ascend logs.\n" +                     \
          c10_npu::acl::AclGetErrMsg();                             \
      throw std::runtime_error(err);                                \
    }                                                               \
  } while (0)

namespace c10d_npu {

// RAII wrapper for HCCL communicator
class HCCLComm {
public:
  explicit HCCLComm(HcclComm hcclComm) : hcclComm_(hcclComm) {}

  HCCLComm() : HCCLComm(nullptr) {}

  ~HCCLComm() {
    destropyHcclComm();
  }

  static std::shared_ptr<HCCLComm> create(
      int numRanks,
      int rank,
      HcclRootInfo& rootInfo) {
    auto comm = std::make_shared<HCCLComm>();
    C10D_HCCL_CHECK(
        HcclCommInitRootInfo(numRanks, &rootInfo, rank, &(comm->hcclComm_)));
    c10_npu::NpuSysCtrl::GetInstance().RegisterReleaseFn([=]() ->void {
          comm->destropyHcclComm();
        }, c10_npu::ReleasePriority::PriorityMiddle);
    return comm;
  }

  // Must not be copyable
  HCCLComm(const HCCLComm&) = delete;
  HCCLComm& operator=(const HCCLComm&) = delete;

  // Move constructable
  HCCLComm(HCCLComm&& other) {
    std::swap(hcclComm_, other.hcclComm_);
  }

  // Move assignable
  HCCLComm& operator=(HCCLComm&& other) {
    std::swap(hcclComm_, other.hcclComm_);
    return *this;
  }

  HcclComm getHcclComm() const{
    return hcclComm_;
  }

  void destropyHcclComm() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (hcclComm_) {
      HcclCommDestroy(hcclComm_);
      hcclComm_ = nullptr;
    }
  }

protected:
  HcclComm hcclComm_;
  mutable std::mutex mutex_;
};
} // namespace c10d_npu
