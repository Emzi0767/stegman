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
#include "zlib.h"
#include "png.h"
#include "steg.h"
#include "encode.h"

// Standard library
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Function definitions
bool encode(const wchar_t *password, size_t passlen, uint8_t *file, size_t filelen, const uint8_t *message, size_t msglen, bool isfile)
{
	uint8_t salt[SALT_SIZE], iv[IV_SIZE], key[KEY_SIZE];

	// Generate salt
	int32_t res = sha_gen_salt(salt);
	if (res)
	{
		werrorf(L"Error generating salt (%lu). Refer to OpenSSL docs for RAND_bytes for more details.\n", res);
		return false;
	}

	// Generate IV
	res = aes_gen_iv(iv);
	if (res)
	{
		werrorf(L"Error generating IV (%lu). Refer to OpenSSL docs for RAND_bytes for more details.\n", res);
		return false;
	}

	// Generate hash cycle count
	srand(time(NULL));
	uint16_t hc = (uint16_t)(rand() % 32768 + 32767);

	// Create the AES key by hashing the password using SHA-256
	res = sha_hash((uint8_t*)password, passlen * sizeof(wchar_t), salt, hc, key);
	if (res)
	{
		werrorf(L"Error generating AES key (%lu). Refer to OpenSSL docs for SHA256 for more details.\n", res);
		return false;
	}

	// Compress the data
	uint8_t *data = NULL;
	uint64_t datalen = 0;
	res = zlib_compress(message, msglen, &data, &datalen);
	if (res)
	{
		werrorf(L"Error compressing data (%d). Refer to ZLib manual for details.\n", res);
		return false;
	}

	// Reallocate the message buffer
	uint32_t isize = sizeof(int32_t);
	uint8_t *data2 = (uint8_t*)calloc(datalen + isize, sizeof(uint8_t));
	if (!data2)
	{
		werrorf(L"Error allocating data buffer (E_MSG_BUFFER_ZLIB_AES).\n");
		return false;
	}
	memcpy(data2 + isize, data, datalen);
	*((int32_t*)data2) = STEG_MAGIC;
	uint64_t data2len = datalen + isize;

	// Encrypt the data
	res = aes_encrypt(data2, data2len, key, iv, &data, &datalen);
	if (res)
	{
		werrorf(L"Error encrypting data (%d). Refer to OpenSSL manual for details.\n", res);
		return false;
	}

	// Load the PNG file pixels
	uint8_t *pixels = NULL;
	uint64_t pixelcount = 0;
	PngImageInfo pnginf;
	res = png_load_pixels(file, filelen, &pixels, &pixelcount, &pnginf);
	if (res)
	{
		werrorf(L"Error loading PNG image (%d). Refer to libpng manual for details.\n", res);
		return false;
	}

	// Prepare steganographic data
	StegMessage smsg;
	steg_init_msg(&smsg);
	smsg.flags = isfile ? MSG_FILE : 0;
	smsg.cycles = hc;
	memcpy(smsg.iv, iv, IV_SIZE);
	memcpy(smsg.salt, salt, SALT_SIZE);
	smsg.length = data2len;
	smsg.contents = (uint8_t*)calloc(datalen, sizeof(uint8_t));
	memcpy(smsg.contents, data, datalen);

	// Steganographically encode the data
	

	// Free memory
	free(smsg.contents);
	free(data2);
	free(data);

	// Return success
	return true;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
