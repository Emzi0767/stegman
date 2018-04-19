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
#include "decode.h"

// Standard library
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function definitions
bool decode(const wchar_t *password, size_t passlen, FILE *png, uint8_t **message, size_t *msglen, bool *isfile)
{
    uint8_t key[KEY_SIZE];

    // Load the PNG data
    uint8_t *pixels = NULL;
	uint64_t pixelcount = 0;
	PngImageInfo pnginf;
	int32_t res = png_load_pixels(png, &pixels, &pixelcount, &pnginf);
	if (res)
	{
		werrorf(L"Error loading PNG image (%d). Refer to libpng manual for details.\n", res);
		return false;
	}

    // Decode the steganographic message
    StegMessage smsg;
    steg_init_msg(&smsg);
    if (!steg_decode(pixels, pixelcount, &smsg))
    {
        free(pixels);
        werrorf(L"Failed to decode data from pixels!\n");
        return false;
    }

    // Set the is file flag
    *isfile = (smsg.flags & MSG_FILE) == MSG_FILE;

    // Create the AES key by hashing the password using SHA-256
	res = sha_hash((uint8_t*)password, passlen * sizeof(wchar_t), smsg.salt, smsg.cycles, key);
	if (res)
	{
        free(smsg.contents);
        free(pixels);
		werrorf(L"Error generating AES key (%lu). Refer to OpenSSL docs for SHA256 for more details.\n", res);
		return false;
	}

    // Decrypt the data
    uint8_t *data = NULL;
	uint64_t datalen = 0;
    res = aes_decrypt(smsg.contents, smsg.length, key, smsg.iv, &data, &datalen);
    if (res)
    {
        free(smsg.contents);
        free(pixels);
		werrorf(L"Error decrypting data (%d). Refer to OpenSSL manual for details.\n", res);
		return false;
    }

    // Check if header matches
    if (*((int32_t*)data) != STEG_MAGIC)
    {
        free(data);
        free(smsg.contents);
        free(pixels);
		werrorf(L"Decrypted data was corrupted or invalid. Did you supply a correct password?\n");
		return false;
    }

    // Decompress the data
    uint8_t *data2 = NULL;
    uint64_t data2len = 0;
    size_t isize = sizeof(int32_t);
    res = zlib_decompress(data + isize, datalen - isize, &data2, &data2len);
    if (res)
    {
        free(data);
        free(smsg.contents);
        free(pixels);
		werrorf(L"Error decompressing data (%d). Refer to ZLib manual for details.\n", res);
        return false;
    }

    // Allocate the target buffer and copy the result
    *message = (uint8_t*)calloc(data2len, sizeof(uint8_t));
    *msglen = data2len;
    memcpy(*message, data2, data2len);

    // Free the memory
    free(data2);
    free(data);
    free(smsg.contents);
    free(pixels);

    return true;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
