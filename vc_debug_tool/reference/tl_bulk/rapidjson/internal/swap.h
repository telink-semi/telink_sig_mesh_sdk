/********************************************************************************************************
 * @file	swap.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef RAPIDJSON_INTERNAL_SWAP_H_
#define RAPIDJSON_INTERNAL_SWAP_H_

#include "../rapidjson.h"

#if defined(__clang__)
RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(c++98-compat)
#endif

RAPIDJSON_NAMESPACE_BEGIN
namespace internal {

//! Custom swap() to avoid dependency on C++ <algorithm> header
/*! \tparam T Type of the arguments to swap, should be instantiated with primitive C++ types only.
    \note This has the same semantics as std::swap().
*/
template <typename T>
inline void Swap(T& a, T& b) RAPIDJSON_NOEXCEPT {
    T tmp = a;
        a = b;
        b = tmp;
}

} // namespace internal
RAPIDJSON_NAMESPACE_END

#if defined(__clang__)
RAPIDJSON_DIAG_POP
#endif

#endif // RAPIDJSON_INTERNAL_SWAP_H_
