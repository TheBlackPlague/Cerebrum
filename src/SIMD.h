//
// Copyright (c) 2023 MantaRay authors. See the list of authors for more details.
// Licensed under MIT.
//

#ifndef MANTARAY_SIMD_H
#define MANTARAY_SIMD_H

#include <array>

#ifdef __AVX512BW__
#include "Backend/Avx512.h"
#elif __AVX2__
#include "Backend/Avx2.h"
#endif

namespace MantaRay
{

    /// \brief SIMD implementations for MantaRay.
    /// \details This class contains SIMD implementations of common operations used in MantaRay.
    class SIMD
    {
        public:
            /// \brief Add the delta to elements in the input arrays.
            /// \tparam T The type of the input and delta.
            /// \tparam InputSize The size of the input arrays.
            /// \tparam DeltaSize The size of the delta array.
            /// \param inputA The first input array.
            /// \param inputB The second input array.
            /// \param delta The delta array.
            /// \param oA The offset into the first input array.
            /// \param oB The offset into the second input array.
            /// \details This function adds the delta to elements in the input arrays starting at the given offsets.
            ///          The delta is added to the input arrays in-place.
            template<typename T, size_t InputSize, size_t DeltaSize>
            static inline void AddToAll(std::array<T, InputSize>& inputA, std::array<T, InputSize>& inputB,
                                        const std::array<T, DeltaSize>& delta,
                                        const uint32_t oA, const uint32_t oB)
            {
#ifdef __AVX512BW__
                Vec512I zmm0;
                Vec512I zmm1;

                for (size_t i = 0; i < InputSize; i += 32) {
                    zmm0 = Avx512<T>::From(inputA, i);
                    zmm1 = Avx512<T>::From(delta, oA + i);
                    zmm0 = Avx512<T>::Add(zmm0, zmm1);
                    Avx512<T>::Store(zmm0, inputA, i);
                }

                for (size_t i = 0; i < InputSize; i += 32) {
                    zmm0 = Avx512<T>::From(inputB, i);
                    zmm1 = Avx512<T>::From(delta, oB + i);
                    zmm0 = Avx512<T>::Add(zmm0, zmm1);
                    Avx512<T>::Store(zmm0, inputB, i);
                }
#elifdef __AVX2__
                Vec256I ymm0;
                Vec256I ymm1;

                for (size_t i = 0; i < InputSize; i += 16) {
                    ymm0 = Avx<T> ::From(inputA, i);
                    ymm1 = Avx<T> ::From(delta, oA + i);
                    ymm0 = Avx2<T>::Add(ymm0, ymm1);
                    Avx<T>::Store(ymm0, inputA, i);
                }

                for (size_t i = 0; i < InputSize; i += 16) {
                    ymm0 = Avx<T> ::From(inputB, i);
                    ymm1 = Avx<T> ::From(delta, oB + i);
                    ymm0 = Avx2<T>::Add(ymm0, ymm1);
                    Avx<T>::Store(ymm0, inputB, i);
                }
#else
                for (size_t i = 0; i < InputSize; i++) inputA[i] += delta[oA + i];
                for (size_t i = 0; i < InputSize; i++) inputB[i] += delta[oB + i];
#endif
            }

            /// \brief Subtract the delta from elements in the input arrays.
            /// \tparam T The type of the input and delta.
            /// \tparam InputSize The size of the input arrays.
            /// \tparam DeltaSize The size of the delta array.
            /// \param inputA The first input array.
            /// \param inputB The second input array.
            /// \param delta The delta array.
            /// \param oA The offset into the first input array.
            /// \param oB The offset into the second input array.
            /// \details This function subtracts the delta from elements in the input arrays starting at the given
            ///          offsets. The delta is subtracted from the input arrays in-place.
            template<typename T, size_t InputSize, size_t DeltaSize>
            static inline void SubtractFromAll(std::array<T, InputSize>& inputA, std::array<T, InputSize>& inputB,
                                               const std::array<T, DeltaSize>& delta,
                                               const uint32_t oA, const uint32_t oB)
            {
#ifdef __AVX512BW__
                Vec512I zmm0;
                Vec512I zmm1;

                for (size_t i = 0; i < InputSize; i += 32) {
                    zmm0 = Avx512<T>::From(inputA, i);
                    zmm1 = Avx512<T>::From(delta, oA + i);
                    zmm0 = Avx512<T>::Subtract(zmm0, zmm1);
                    Avx512<T>::Store(zmm0, inputA, i);
                }

                for (size_t i = 0; i < InputSize; i += 32) {
                    zmm0 = Avx512<T>::From(inputB, i);
                    zmm1 = Avx512<T>::From(delta, oB + i);
                    zmm0 = Avx512<T>::Subtract(zmm0, zmm1);
                    Avx512<T>::Store(zmm0, inputB, i);
                }
#elifdef __AVX2__
                Vec256I ymm0;
                Vec256I ymm1;

                for (size_t i = 0; i < InputSize; i += 16) {
                    ymm0 = Avx<T> ::From(inputA, i);
                    ymm1 = Avx<T> ::From(delta, oA + i);
                    ymm0 = Avx2<T>::Subtract(ymm0, ymm1);
                    Avx<T>::Store(ymm0, inputA, i);
                }

                for (size_t i = 0; i < InputSize; i += 16) {
                    ymm0 = Avx<T> ::From(inputB, i);
                    ymm1 = Avx<T> ::From(delta, oB + i);
                    ymm0 = Avx2<T>::Subtract(ymm0, ymm1);
                    Avx<T>::Store(ymm0, inputB, i);
                }
#else
                for (size_t i = 0; i < InputSize; i++) inputA[i] -= delta[oA + i];
                for (size_t i = 0; i < InputSize; i++) inputB[i] -= delta[oB + i];
#endif
            }

            /// \brief Combination of SubtractFromAll and AddToAll.
            /// \tparam T The type of the input and delta.
            /// \tparam InputSize The size of the input arrays.
            /// \tparam DeltaSize The size of the delta array.
            /// \param inputA The first input array.
            /// \param inputB The second input array.
            /// \param delta The delta array.
            /// \param oAS The offset into the first input array for the subtraction.
            /// \param oAA The offset into the first input array for the addition.
            /// \param oBS The offset into the second input array for the subtraction.
            /// \param oBA The offset into the second input array for the addition.
            /// \details This function subtracts the delta from elements in the input arrays starting at the given
            ///          offsets. The delta is subtracted from the input arrays in-place. The delta is then added to
            ///          the input arrays starting at the other given offsets. The delta is added to the input arrays
            ///          in-place.
            template<typename T, size_t InputSize, size_t DeltaSize>
            static inline void SubtractAndAddToAll(std::array<T, InputSize>& inputA, std::array<T, InputSize>& inputB,
                                                   const std::array<T, DeltaSize>& delta,
                                                   const uint32_t oAS, const uint32_t oAA,
                                                   const uint32_t oBS, const uint32_t oBA)
            {
#ifdef __AVX512BW__
                Vec512I zmm0;
                Vec512I zmm1;
                Vec512I zmm2;

                for (size_t i = 0; i < InputSize; i += 32) {
                    zmm0 = Avx512<T>::From(inputA, i);
                    zmm1 = Avx512<T>::From(delta, oAS + i);
                    zmm2 = Avx512<T>::From(delta, oAA + i);
                    zmm0 = Avx512<T>::Subtract(zmm0, zmm1);
                    zmm0 = Avx512<T>::Add(zmm0, zmm2);
                    Avx512<T>::Store(zmm0, inputA, i);
                }

                for (size_t i = 0; i < InputSize; i += 32) {
                    zmm0 = Avx512<T>::From(inputB, i);
                    zmm1 = Avx512<T>::From(delta, oBS + i);
                    zmm2 = Avx512<T>::From(delta, oBA + i);
                    zmm0 = Avx512<T>::Subtract(zmm0, zmm1);
                    zmm0 = Avx512<T>::Add(zmm0, zmm2);
                    Avx512<T>::Store(zmm0, inputB, i);
                }
#elifdef __AVX2__
                Vec256I ymm0;
                Vec256I ymm1;
                Vec256I ymm2;

                for (size_t i = 0; i < InputSize; i += 16) {
                    ymm0 = Avx<T> ::From(inputA, i);
                    ymm1 = Avx<T> ::From(delta, oAS + i);
                    ymm2 = Avx<T> ::From(delta, oAA + i);
                    ymm0 = Avx2<T>::Subtract(ymm0, ymm1);
                    ymm0 = Avx2<T>::Add(ymm0, ymm2);
                    Avx<T>::Store(ymm0, inputA, i);
                }

                for (size_t i = 0; i < InputSize; i += 16) {
                    ymm0 = Avx<T> ::From(inputB, i);
                    ymm1 = Avx<T> ::From(delta, oBS + i);
                    ymm2 = Avx<T> ::From(delta, oBA + i);
                    ymm0 = Avx2<T>::Subtract(ymm0, ymm1);
                    ymm0 = Avx2<T>::Add(ymm0, ymm2);
                    Avx<T>::Store(ymm0, inputB, i);
                }
#else
                for (size_t i = 0; i < InputSize; i++) {
                    inputA[i] = inputA[i] - delta[oAS + i] + delta[oAA + i];
                    inputB[i] = inputB[i] - delta[oBS + i] + delta[oBA + i];
                }
#endif
            }

            /// \brief Activate the input arrays, flatten the concatenated tensor result, and forward propagate the
            ///        flattened result.
            /// \tparam Activation The activation function to use.
            /// \tparam T The type of the input, weight, and bias arrays.
            /// \tparam OT The type of the output array.
            /// \tparam InputSize The size of the input arrays.
            /// \tparam OutputSize The size of the output array.
            /// \param inputA The first input array.
            /// \param inputB The second input array.
            /// \param weight The weight array.
            /// \param bias The bias array.
            /// \param output The output array.
            /// \param o The offset into the output array.
            /// \details This function activates the input arrays. Then it creates a Tensor-view of the input arrays,
            ///          concatenating them vertically. After which, it flattens the vertical tensor into a 1D tensor.
            ///          Finally, it forwards propagates the flattened tensor using the weight and bias arrays using
            ///          simple matrix multiplication. The result is stored in the output array starting at the given
            ///          offset.
            template<typename Activation, typename T, typename OT, size_t InputSize, size_t OutputSize>
            [[clang::noinline]]
            static void ActivateFlattenAndForward(
                    const std::array<T, InputSize>& inputA, const std::array<T, InputSize>& inputB,
                    const std::array<T, InputSize * 2 * OutputSize>& weight,
                    const std::array<T, OutputSize>& bias,
                    std::array<OT, OutputSize>& output, const uint32_t o)
            {
                size_t stride = 0;
                for (size_t i = 0; i < OutputSize; i++) {
#ifdef __AVX512BW__
                    Vec512I zmm0 = Avx512<OT>::Zero();
                    Vec512I zmm1;
                    Vec512I zmm2;

                    for (size_t j = 0; j < InputSize; j += 32) {
                        // START INPUT A
                        zmm1 = Avx512<T> ::From(inputA, j);
                        zmm2 = Avx512<T> ::From(weight, stride + j);
                        zmm1 = Activation::Activate(zmm1);
                        zmm1 = Avx512<T> ::MultiplyAndAddAdjacent(zmm1, zmm2);
                        zmm0 = Avx512<OT>::Add(zmm0, zmm1);
                        // END INPUT A

                        // START INPUT B
                        zmm1 = Avx512<T> ::From(inputB, j);
                        zmm2 = Avx512<T> ::From(weight, InputSize + stride + j);
                        zmm1 = Activation::Activate(zmm1);
                        zmm1 = Avx512<T> ::MultiplyAndAddAdjacent(zmm1, zmm2);
                        zmm0 = Avx512<OT>::Add(zmm0, zmm1);
                        // END INPUT B
                    }

                    stride += InputSize * 2;

                    output[o + i] = Avx512<OT>::Sum(zmm0) + bias[o + i];
#elifdef __AVX2__
                    Vec256I ymm0 = Avx<OT>::Zero();
                    Vec256I ymm1;
                    Vec256I ymm2;

                    for (size_t j = 0; j < InputSize; j += 16) {
                        // START INPUT A
                        ymm1 = Avx<T>    ::From(inputA, j);
                        ymm2 = Avx<T>    ::From(weight, stride + j);
                        ymm1 = Activation::Activate(ymm1);
                        ymm1 = Avx2<T>   ::MultiplyAndAddAdjacent(ymm1, ymm2);
                        ymm0 = Avx2<OT>  ::Add(ymm0, ymm1);
                        // END INPUT A

                        // START INPUT B
                        ymm1 = Avx<T>    ::From(inputB, j);
                        ymm2 = Avx<T>    ::From(weight, InputSize + stride + j);
                        ymm1 = Activation::Activate(ymm1);
                        ymm1 = Avx2<T>   ::MultiplyAndAddAdjacent(ymm1, ymm2);
                        ymm0 = Avx2<OT>  ::Add(ymm0, ymm1);
                        // END INPUT B
                    }

                    stride += InputSize * 2;

                    output[o + i] = Avx2<OT>::Sum(ymm0) + bias[o + i];
#else
                    OT sum = 0;

                    for (size_t j = 0; j < InputSize; j++) {
                        sum += Activation::Activate(inputA[j]) * weight[stride + j];
                        sum += Activation::Activate(inputB[j]) * weight[InputSize + stride + j];
                    }

                    stride += InputSize * 2;
                    output[o + i] = sum + bias[o + i];
#endif
                }
            }

    };
} // MantaRay

#endif //MANTARAY_SIMD_H
