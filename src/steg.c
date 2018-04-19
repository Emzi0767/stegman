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

// Appropriate headers
#include "defs.h"
#include "aes.h"
#include "sha256.h"
#include "steg.h"

// Standard library
#include <stdlib.h>
#include <string.h>

// Constant definitions
const int32_t STEG_MAGIC = 0x0BADFACE;

// Helper functions and constants
const uint8_t BITMASK = ~0x03;
const uint8_t BITDATA = 0x03;
static inline void encode_byte(uint8_t byte, uint8_t *ptr)
{
	uint8_t t = byte;
	for (int i = 0; i < 4; i++)
	{
		ptr[i] &= BITMASK;
		ptr[i] |= (t >> ((3 - i) * 2)) & BITDATA;
	}
}

static inline void decode_byte(uint8_t *ptr, uint8_t *byte)
{
	uint8_t t = 0;
	for (int i = 0; i < 4; i++)
		t |= (ptr[i] & BITDATA) << ((3 - i) * 2);
	
	*byte = t;
}

// Function definitions
void steg_init_msg(StegMessage *msg)
{
	int32_t magic = STEG_MAGIC;
	memcpy(msg, &magic, sizeof(magic));
}

bool steg_encode(const StegMessage *data, uint8_t *pixels, size_t pixellen)
{
	uint8_t *cpx = pixels;
	uint64_t ctr = 0, len = data->length;

	if (len % 16)
		len = ((len / 16) + 1) * 16;

	// Encode magic
	encode_byte(data->magic >> 24, cpx + (ctr++ * 4));
	encode_byte(data->magic >> 16, cpx + (ctr++ * 4));
	encode_byte(data->magic >> 8, cpx + (ctr++ * 4));
	encode_byte(data->magic, cpx + (ctr++ * 4));

	// Encode flags
	encode_byte(data->flags >> 24, cpx + (ctr++ * 4));
	encode_byte(data->flags >> 16, cpx + (ctr++ * 4));
	encode_byte(data->flags >> 8, cpx + (ctr++ * 4));
	encode_byte(data->flags, cpx + (ctr++ * 4));

	// Encode hash cycle count
	encode_byte(data->cycles >> 8, cpx + (ctr++ * 4));
	encode_byte(data->cycles, cpx + (ctr++ * 4));

	// Encode iv
	for (int8_t i = 0; i < IV_SIZE; i++)
		encode_byte(data->iv[i], cpx + (ctr++ * 4));

	// Encode salt
	for (int8_t i = 0; i < SALT_SIZE; i++)
		encode_byte(data->salt[i], cpx + (ctr++ * 4));

	// Encode length
	encode_byte(data->length >> 56, cpx + (ctr++ * 4));
	encode_byte(data->length >> 48, cpx + (ctr++ * 4));
	encode_byte(data->length >> 40, cpx + (ctr++ * 4));
	encode_byte(data->length >> 32, cpx + (ctr++ * 4));
	encode_byte(data->length >> 24, cpx + (ctr++ * 4));
	encode_byte(data->length >> 16, cpx + (ctr++ * 4));
	encode_byte(data->length >> 8, cpx + (ctr++ * 4));
	encode_byte(data->length, cpx + (ctr++ * 4));

	// Encode content
	cpx += ctr * 4;
	uint8_t *cptr = data->contents;
	for (uint64_t i = 0; i < len; i++)
	{
		encode_byte(*cptr, cpx);

		cptr++;
		cpx += 4;
		if (cpx > (pixels + pixellen))
			return false;
	}

	return true;
}

bool steg_decode(const uint8_t *pixels, size_t pixellen, StegMessage *data)
{
	uint8_t *cpx = pixels, *bptr = NULL;
	uint64_t ctr = 0;

	// Decode and verify the magic
	int32_t magic = 0;
	bptr = (uint8_t*)(&magic);
	for (int8_t i = 0; i < sizeof(int32_t); i++)
		decode_byte(cpx + (ctr++ * 4), bptr + i);
	
	if (magic != STEG_MAGIC)
		return false;
	
	// Decode message flags
	StegMessageFlags flags = MSG_NONE;
	bptr = (uint8_t*)(&flags);
	for (int8_t i = 0; i < sizeof(int32_t); i++)
		decode_byte(cpx + (ctr++ * 4), bptr + i);
	
	data->flags = flags;

	// Decode hash cycle count
	uint16_t hcc = 0;
	bptr = (uint8_t*)(&hcc);
	for (int8_t i = 0; i < sizeof(uint16_t); i++)
		decode_byte(cpx + (ctr++ * 4), bptr + i);
	
	data->cycles = hcc;

	// Decode iv
	for (int8_t i = 0; i < IV_SIZE; i++)
		decode_byte(cpx + (ctr++ * 4), data->iv + i);
	
	// Decode salt
	for (int8_t i = 0; i < SALT_SIZE; i++)
		decode_byte(cpx + (ctr++ * 4), data->salt + i);
	
	// Decode message length
	uint64_t len = 0;
	bptr = (uint8_t*)(&len);
	for (int8_t i = 0; i < sizeof(uint64_t); i++)
		decode_byte(cpx + (ctr++ * 4), bptr + i);

	data->length = len;

	// Round to block size for decryption purposes
	if (len % 16)
		len = ((len / 16) + 1) * 16;
	
	// Decode encrypted contents
	cpx += ctr * 4;
	data->contents = (uint8_t*)calloc(len, sizeof(uint8_t));
	uint8_t *cptr = data->contents;
	for (uint64_t i = 0; i < len; i++)
	{
		decode_byte(cpx, cptr);

		cptr++;
		cpx += 4;
		if (cpx > (pixels + pixellen))
			return false;
	}

	return true;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
