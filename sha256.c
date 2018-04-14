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
#include "sha256.h"

// Standard library
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/err.h>

// Constant definitions
const int32_t SALT_SIZE = 16;
const int32_t DIGEST_SIZE = 32;

// Function definitions
int32_t sha_gen_salt(uint8_t salt[SALT_SIZE])
{
	int32_t res = RAND_bytes(salt, SALT_SIZE);
	if (!res)
		return ERR_get_error();
	
	return 0;
}

int32_t sha_hash(const uint8_t *msg, size_t len, uint8_t salt[SALT_SIZE], uint16_t cycles, uint8_t result[DIGEST_SIZE])
{
	uint8_t *tmp = (uint8_t*)calloc(len + SALT_SIZE, sizeof(uint8_t));

	if (!tmp)
		return ERR_get_error();

	memcpy(tmp, msg, len * sizeof(uint8_t));
	memcpy(tmp + len, salt, SALT_SIZE);

	for (uint16_t i = 0; i < cycles; i++)
	{
		SHA256_CTX sha256;
		if (!SHA256_Init(&sha256)
			|| !SHA256_Update(&sha256, tmp, len)
			|| !SHA256_Final(result, &sha256))
		{
			free(tmp);
			return ERR_get_error();
		}

		if (i == 0)
		{
			uint8_t *tmp2 = (uint8_t*)realloc(tmp, DIGEST_SIZE + SALT_SIZE);
			if (!tmp2)
			{
				free(tmp);
				return ERR_get_error();
			}

			tmp = tmp2;
			memcpy(tmp + DIGEST_SIZE, salt, SALT_SIZE);
		}

		memcpy(tmp, result, DIGEST_SIZE);
	}

	free(tmp);
	return 0;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
