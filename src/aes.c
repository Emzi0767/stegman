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

// Standard library
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/err.h>

// Constant definitions
const int32_t KEY_SIZE = 32;
const int32_t IV_SIZE = 16;

// Function definitions
int32_t aes_gen_iv(uint8_t iv[IV_SIZE])
{
	int32_t res = RAND_bytes(iv, IV_SIZE);
	if (!res)
		return ERR_get_error();
	
	return 0;
}

int32_t aes_encrypt(const uint8_t *msg, uint64_t len, const uint8_t key[KEY_SIZE], const uint8_t iv[IV_SIZE], uint8_t **result, uint64_t *reslen)
{
	// Calculate output length
	if (len % AES_BLOCK_SIZE)
		*reslen = ((len / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
	else
		*reslen = len;
	
	// Allocate output
	*result = (uint8_t*)calloc(*reslen, sizeof(uint8_t));
	if (!result)
		return 1;

	// Initialize AES-256 key
	AES_KEY aes_key;
	AES_set_encrypt_key(key, 256, &aes_key);

	// Encrypt with AES-CBC
	AES_cbc_encrypt(msg, *result, len, &aes_key, iv, AES_ENCRYPT);

	return 0;
}

int32_t aes_decrypt(const uint8_t *msg, uint64_t len, const uint8_t key[KEY_SIZE], const uint8_t iv[IV_SIZE], uint8_t **result, uint64_t *reslen)
{
	// Calculate input length
	uint64_t elen = 0;
	if (len % AES_BLOCK_SIZE)
		elen = ((len / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
	else
		elen = len;
	
	// Allocate output
	*reslen = len;
	*result = (uint8_t*)calloc(elen, sizeof(uint8_t));
	if (!result)
		return 1;

	// Initialize AES-256 key
	AES_KEY aes_key;
	AES_set_decrypt_key(key, 256, &aes_key);

	// Decrypt with AES-CBC
	AES_cbc_encrypt(msg, *result, elen, &aes_key, iv, AES_DECRYPT);

	return 0;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
