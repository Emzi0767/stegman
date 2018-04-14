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

// Check if we're using a C99-capable compiler
#if __STDC_VERSION__ < 199901L
#error "You need to use a C99-capable compiler to build this program!"
#endif

// Define C extern for C++
#ifdef __cplusplus
extern "C"
{
#endif

// Appropriate header
#include "defs.h"
#include "zlib.h"

// Standard library
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

// Helper
static inline uint64_t max(uint64_t a, uint64_t b)
{
	return a > b ? a : b;
}

// Function definitions
int32_t zlib_compress(const uint8_t *data, uint64_t length, uint8_t **result, uint64_t *reslen)
{
	uint64_t buflen = max(length * 2, length + 256);
	*reslen = buflen;
	*result = (uint8_t*)calloc(buflen, sizeof(uint8_t));
	if (!(*result))
	{
		return 16;
	}

	int32_t res = compress2(*result, &buflen, data, length, Z_BEST_COMPRESSION);
	if (!res)
	{
		free(*result);
		return res;
	}

	if (buflen != *reslen)
	{
		uint8_t *result2 = realloc(*result, buflen);
		if (!result2)
		{
			free(*result);
			return 32;
		}

		*result = result2;
	}

	return res;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
