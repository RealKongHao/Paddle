/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <cudnn.h>
#include <dlfcn.h>
#include <mutex>
#include "paddle/platform/dynload/dynamic_loader.h"

namespace paddle {
namespace platform {
namespace dynload {

extern std::once_flag cudnn_dso_flag;
extern void* cudnn_dso_handle;
extern bool HasCUDNN();

#ifdef PADDLE_USE_DSO

extern void EnforceCUDNNLoaded(const char* fn_name);
#define DECLARE_DYNAMIC_LOAD_CUDNN_WRAP(__name)                    \
  struct DynLoad__##__name {                                       \
    template <typename... Args>                                    \
    auto operator()(Args... args) -> decltype(__name(args...)) {   \
      using cudnn_func = decltype(__name(args...)) (*)(Args...);   \
      std::call_once(cudnn_dso_flag,                               \
                     paddle::platform::dynload::GetCudnnDsoHandle, \
                     &cudnn_dso_handle);                           \
      EnforceCUDNNLoaded(#__name);                                 \
      void* p_##__name = dlsym(cudnn_dso_handle, #__name);         \
      return reinterpret_cast<cudnn_func>(p_##__name)(args...);    \
    }                                                              \
  };                                                               \
  extern struct DynLoad__##__name __name

#else

#define DECLARE_DYNAMIC_LOAD_CUDNN_WRAP(__name)                  \
  struct DynLoad__##__name {                                     \
    template <typename... Args>                                  \
    auto operator()(Args... args) -> decltype(__name(args...)) { \
      return __name(args...);                                    \
    }                                                            \
  };                                                             \
  extern DynLoad__##__name __name

#endif

/**
 * include all needed cudnn functions in HPPL
 * different cudnn version has different interfaces
 **/
#define CUDNN_DNN_ROUTINE_EACH(__macro)             \
  __macro(cudnnSetTensor4dDescriptor);              \
  __macro(cudnnSetTensor4dDescriptorEx);            \
  __macro(cudnnSetTensorNdDescriptor);              \
  __macro(cudnnGetTensorNdDescriptor);              \
  __macro(cudnnGetConvolutionNdForwardOutputDim);   \
  __macro(cudnnGetConvolutionForwardAlgorithm);     \
  __macro(cudnnCreateTensorDescriptor);             \
  __macro(cudnnDestroyTensorDescriptor);            \
  __macro(cudnnCreateFilterDescriptor);             \
  __macro(cudnnSetFilter4dDescriptor);              \
  __macro(cudnnSetFilterNdDescriptor);              \
  __macro(cudnnGetFilterNdDescriptor);              \
  __macro(cudnnSetPooling2dDescriptor);             \
  __macro(cudnnSetPoolingNdDescriptor);             \
  __macro(cudnnGetPoolingNdDescriptor);             \
  __macro(cudnnDestroyFilterDescriptor);            \
  __macro(cudnnCreateConvolutionDescriptor);        \
  __macro(cudnnCreatePoolingDescriptor);            \
  __macro(cudnnDestroyPoolingDescriptor);           \
  __macro(cudnnSetConvolution2dDescriptor);         \
  __macro(cudnnDestroyConvolutionDescriptor);       \
  __macro(cudnnSetConvolutionNdDescriptor);         \
  __macro(cudnnGetConvolutionNdDescriptor);         \
  __macro(cudnnDeriveBNTensorDescriptor);           \
  __macro(cudnnCreate);                             \
  __macro(cudnnDestroy);                            \
  __macro(cudnnSetStream);                          \
  __macro(cudnnActivationForward);                  \
  __macro(cudnnConvolutionForward);                 \
  __macro(cudnnConvolutionBackwardBias);            \
  __macro(cudnnGetConvolutionForwardWorkspaceSize); \
  __macro(cudnnTransformTensor);                    \
  __macro(cudnnPoolingForward);                     \
  __macro(cudnnPoolingBackward);                    \
  __macro(cudnnSoftmaxBackward);                    \
  __macro(cudnnSoftmaxForward);                     \
  __macro(cudnnGetVersion);                         \
  __macro(cudnnGetErrorString);
CUDNN_DNN_ROUTINE_EACH(DECLARE_DYNAMIC_LOAD_CUDNN_WRAP)

#define CUDNN_DNN_ROUTINE_EACH_R2(__macro) \
  __macro(cudnnAddTensor);                 \
  __macro(cudnnConvolutionBackwardData);   \
  __macro(cudnnConvolutionBackwardFilter);
CUDNN_DNN_ROUTINE_EACH_R2(DECLARE_DYNAMIC_LOAD_CUDNN_WRAP)

// APIs available after R3:
#if CUDNN_VERSION >= 3000
#define CUDNN_DNN_ROUTINE_EACH_AFTER_R3(__macro)           \
  __macro(cudnnGetConvolutionBackwardFilterWorkspaceSize); \
  __macro(cudnnGetConvolutionBackwardDataAlgorithm);       \
  __macro(cudnnGetConvolutionBackwardFilterAlgorithm);     \
  __macro(cudnnGetConvolutionBackwardDataWorkspaceSize);
CUDNN_DNN_ROUTINE_EACH_AFTER_R3(DECLARE_DYNAMIC_LOAD_CUDNN_WRAP)
#endif

// APIs available after R4:
#if CUDNN_VERSION >= 4007
#define CUDNN_DNN_ROUTINE_EACH_AFTER_R4(__macro)    \
  __macro(cudnnBatchNormalizationForwardTraining);  \
  __macro(cudnnBatchNormalizationForwardInference); \
  __macro(cudnnBatchNormalizationBackward);
CUDNN_DNN_ROUTINE_EACH_AFTER_R4(DECLARE_DYNAMIC_LOAD_CUDNN_WRAP)
#endif

// APIs in R5
#if CUDNN_VERSION >= 5000
#define CUDNN_DNN_ROUTINE_EACH_R5(__macro)  \
  __macro(cudnnCreateActivationDescriptor); \
  __macro(cudnnSetActivationDescriptor);    \
  __macro(cudnnGetActivationDescriptor);    \
  __macro(cudnnDestroyActivationDescriptor);
CUDNN_DNN_ROUTINE_EACH_R5(DECLARE_DYNAMIC_LOAD_CUDNN_WRAP)
#endif

#if CUDNN_VERSION >= 7001
#define CUDNN_DNN_ROUTINE_EACH_R7(__macro) \
  __macro(cudnnSetConvolutionGroupCount);
CUDNN_DNN_ROUTINE_EACH_R7(DECLARE_DYNAMIC_LOAD_CUDNN_WRAP)
#endif

}  // namespace dynload
}  // namespace platform
}  // namespace paddle
