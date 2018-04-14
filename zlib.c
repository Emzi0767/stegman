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
static inline uint64_t min(uint64_t a, uint64_t b)
{
	return a < b ? a : b;
}

// Function definitions
bool zlib_compress(const uint8_t *data, uint64_t length, uint8_t **result, uint64_t *reslen)
{
	const int32_t CHUNK_SIZE = 256 * 1024;
	int32_t ret = 0, flush = 0;
	uint32_t compd = 0;
	uint64_t tmp = 0;
	z_stream stream;
	uint8_t in[CHUNK_SIZE], out[CHUNK_SIZE];

	// Initialize Zlib
	stream.zalloc = NULL;
	stream.zfree = NULL;
	stream.opaque = NULL;
	ret = deflateInit(&stream, 9); // compression level 9
	if (ret != Z_OK)
		return false;

	// Allocate initial memory block
	*result = (uint8_t*)calloc(length, sizeof(uint8_t));

	// Compress!
	const uint8_t *dat = data;
	uint64_t len = length, clen = 0, plen = length;
	do
	{
		// Copy input
		tmp = min(CHUNK_SIZE, len);
		memcpy(in, dat, tmp);

		// Set input parameters
		stream.avail_in = tmp;
		stream.next_in = in;
		flush = stream.avail_in == CHUNK_SIZE ? Z_NO_FLUSH : Z_FINISH;

		// Advance input pointer
		dat += CHUNK_SIZE;
		len -= CHUNK_SIZE;

		do
		{
			// Set output parameters
			stream.avail_out = CHUNK_SIZE;
			stream.next_out = out;

			// Deflate
			ret = deflate(&stream, flush);
			if (ret != Z_OK)
			{
				deflateEnd(&stream);
				return false;
			}

			// Check if we can fit
			compd = CHUNK_SIZE - stream.avail_out;
			clen += compd;
			while (clen > plen)
			{
				plen += CHUNK_SIZE;
				uint8_t *res2 = (uint8_t*)realloc(*result, plen * sizeof(uint8_t));
				if (!res2)
				{
					deflateEnd(&stream);
					return false;
				}

				*result = res2;
			}

			// Copy compressed data to output
			memcpy(*result, out, compd);
		}
		while (stream.avail_out == 0);
	}
	while (flush != Z_STREAM_END);

	// Truncate buffer if necessary
	if (plen != clen)
	{
		uint8_t *res2 = (uint8_t*)realloc(*result, clen * sizeof(uint8_t));
		if (!res2)
		{
			deflateEnd(&stream);
			return false;
		}

		*result = res2;
	}
	*reslen = clen;

	// Finish compression and return success
	deflateEnd(&stream);
	return true;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
