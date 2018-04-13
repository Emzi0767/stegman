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

// Steganography data magic
const int32_t STEG_MAGIC = 0x0BADFACE;

// Message setting flags
typedef enum StegMessageFlags
{
	// The source message was a file
	MSG_FILE = 1
} StegMessageFlags;

// Structure holding the message
typedef struct StegMessage
{
	// 0x0BADFACE
	const int32_t magic;

	// Setting flags of the message
	StegMessageFlags flags;

	// Number of cycles to hash the password
	uint16_t cycles;

	// Initialization vector for AES
	uint8_t iv[16];

	// Salt to use for password
	uint8_t salt[16];

	// Length of the encoded data
	uint64_t length;

	// Pointer to data
	uint8_t *contents;
} StegMessage;

/*
 * Initializes given StegMessage with proper constants.
 *
 * msg: Pointer to the message to initialize.
 */
void steg_init_msg(StegMessage *msg);

/*
 * Encodes supplied data in the supplied pixel array.
 *
 * data: Data to encode in the pixels.
 * pixels: Pixels to encode the data in.
 * pixellen: Length of the encoding pixel array.
 *
 * returns: Whether the operation succeded.
 */
bool steg_encode(const StegMessage *data, uint8_t *pixels, size_t pixellen);

/*
 * Decodes supplied data from the supplied pixel array.
 *
 * pixels: Pixels to decode the data from.
 * pixellen: Length of the decoded pixel array.
 * data: Pointer to the structure with decoded data.
 *
 * returns: Whether the operation succeeded.
 */
bool steg_decode(const uint8_t *pixels, size_t pixellen, StegMessage *data);

// Define C extern for C++
#ifdef __cplusplus
}
#endif
