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

#include "torch_npu/csrc/framework/utils/OpAdapter.h"
#include "torch_npu/csrc/framework/utils/CalcuOpUtil.h"
#include "torch_npu/csrc/aten/NPUNativeFunctions.h"

namespace at_npu
{
  namespace native
  {

    at::Tensor &eq_out_npu_nocheck(at::Tensor &result, const at::Tensor &self, const at::Tensor &other)
    {
      at::Tensor selfCast = self;
      at::Tensor otherCast = other;
      if (self.dtype() == c10::ScalarType::Int || other.dtype() == c10::ScalarType::Int)
      {
        selfCast = self.to(c10::ScalarType::Float);
        otherCast = other.to(c10::ScalarType::Float);
      }
      auto unified_result = OpPreparation::comparison_op_check(result, selfCast, otherCast, true);
      OpCommand cmd;
      cmd.Name("Equal")
          .Expect(unified_result)
          .Input(selfCast)
          .Input(otherCast)
          .Output(result)
          .Run();

      return result;
    }

    at::Tensor &eq_out_npu_nocheck(at::Tensor &result, const at::Tensor &self, at::Scalar other)
    {
      at::Tensor selfCast = self;
      if (self.dtype() == c10::ScalarType::Int)
      {
        selfCast = self.to(c10::ScalarType::Float);
      }
      OpCommand cmd;
      cmd.Name("Equal")
          .Input(selfCast)
          .Input(other, selfCast.scalar_type())
          .Output(result)
          .Run();

      return result;
    }

    at::Tensor &NPUNativeFunctions::eq_out(const at::Tensor &self, const at::Tensor &other, at::Tensor &result)
    {
      auto outputSize = broadcast_ops_npu_output_size(self, other);
      OpPreparation::CheckOut(
          {self, other},
          result,
          ACL_FORMAT_ND,
          result.scalar_type(),
          at::IntArrayRef(outputSize));
      eq_out_npu_nocheck(result, self, other);
      return result;
    }

    at::Tensor &NPUNativeFunctions::eq_out(const at::Tensor &self, at::Scalar other, at::Tensor &result)
    {
      OpPreparation::CheckOut(
          {self},
          result,
          ACL_FORMAT_ND,
          result.scalar_type(),
          self.sizes());
      eq_out_npu_nocheck(result, self, other);
      return result;
    }

    at::Tensor NPUNativeFunctions::eq(const at::Tensor &self, const at::Tensor &other)
    {
      at::Tensor formatCastOfSelf = OpPreparation::CastBackToOriFormat(self);
      at::Tensor formatCastOfOther = OpPreparation::CastBackToOriFormat(other);

      // calculate the output size
      auto outputSize = broadcast_ops_npu_output_size(formatCastOfSelf, formatCastOfOther);

      // construct the output tensor of the NPU
      at::Tensor result = NPUNativeFunctions::empty_with_format(
          outputSize,
          formatCastOfSelf.options().dtype(at::kBool),
          ACL_FORMAT_ND);

      // calculate the output result of the NPU
      eq_out_npu_nocheck(result, formatCastOfSelf, formatCastOfOther);
      return result;
    }

    at::Tensor NPUNativeFunctions::eq(const at::Tensor &self, at::Scalar other)
    {
      at::Tensor formatCastOfSelf = OpPreparation::CastBackToOriFormat(self);

      // calculate the output size
      auto outputSize = input_same_output_size(formatCastOfSelf);

      // construct the output tensor of the NPU
      at::Tensor result = NPUNativeFunctions::empty_with_format(
          outputSize,
          formatCastOfSelf.options().dtype(at::kBool),
          ACL_FORMAT_ND);

      // calculate the output result of the NPU
      eq_out_npu_nocheck(result, formatCastOfSelf, other);
      return result;
    }

    at::Tensor &eq_npu_(at::Tensor &self, const at::Tensor &other)
    {
      OpPreparation::CastBackToOriFormat(self);
      at::Tensor formatCastOfOther = OpPreparation::CastBackToOriFormat(other);
      c10::SmallVector<at::Tensor, N> inputs = {self, formatCastOfOther};
      c10::SmallVector<at::Tensor, N> outputs = {self};
      CalcuOpUtil::check_memory_over_laps(inputs, outputs);

      at::Tensor result = NPUNativeFunctions::empty_with_format(
          self.sizes(),
          self.options().dtype(c10::ScalarType::Byte),
          CalcuOpUtil::get_tensor_npu_format(self));

      if (!NpuUtils::check_match(&self))
      {
        at::Tensor contiguousSelf = NpuUtils::format_contiguous(self);
        eq_out_npu_nocheck(result, contiguousSelf, formatCastOfOther);
      }
      else
      {
        eq_out_npu_nocheck(result, self, formatCastOfOther);
      }

      // uint8 to self dtype
      NPUNativeFunctions::npu_dtype_cast_(self, result);

      return self;
    }

    at::Tensor &eq_npu_(at::Tensor &self, at::Scalar other)
    {
      OpPreparation::CastBackToOriFormat(self);
      c10::SmallVector<at::Tensor, N> inputs = {self};
      c10::SmallVector<at::Tensor, N> outputs = {self};
      CalcuOpUtil::check_memory_over_laps(inputs, outputs);

      at::Tensor result = NPUNativeFunctions::empty_with_format(
          self.sizes(),
          self.options().dtype(c10::ScalarType::Byte),
          CalcuOpUtil::get_tensor_npu_format(self));

      if (!NpuUtils::check_match(&self))
      {
        at::Tensor contiguousSelf = NpuUtils::format_contiguous(self);
        eq_out_npu_nocheck(result, contiguousSelf, other);
      }
      else
      {
        eq_out_npu_nocheck(result, self, other);
      }

      // uint8 to self dtype
      NPUNativeFunctions::npu_dtype_cast_(self, result);

      return self;
    }

  } // namespace native
} // namespace at_npu
