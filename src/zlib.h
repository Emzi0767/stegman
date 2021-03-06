// This file is part of stegman project
//
// Copyright (c) 2018 Mateusz Brawański (Emzi0767)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * \file
 * \brief ZLib wrapper for easy ZLib operation.
 */

// Only include once
#pragma once

// Check if we're using a C99-capable compiler
#if __STDC_VERSION__ < 199901L
#error "You need to use a C99-capable compiler to build this program!"
#endif

// Define C extern for C++
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Zlib-compresses supplied data.
 *
 * \param data Data to compress.
 * \param length Length of the data.
 * \param result Pointer to result bytes. The underlying pointer will be initialized.
 * \param reslen Pointer to result length. It will be set to length of compressed data.
 *
 * \return 0 if the operation was successful, an error code otherwise.
 */
int32_t zlib_compress(const uint8_t *data, uint64_t length, uint8_t **result, uint64_t *reslen);

/**
 * Zlib-decompresses supplied data.
 *
 * \param data Data to decompress.
 * \param length Length of the data.
 * \param result Pointer to result bytes. The underlying pointer will be initialized.
 * \param reslen Pointer to result length. It will be set to length of resulting data.
 *
 * \return 0 if the operation was successful, an error code otherwise.
 */
int32_t zlib_decompress(const uint8_t *data, uint64_t length, uint8_t **result, uint64_t *reslen);

// Define C extern for C++
#ifdef __cplusplus
}
#endif
