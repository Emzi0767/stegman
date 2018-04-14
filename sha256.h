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

// The size of the SHA-256 salt
extern const int32_t SALT_SIZE;

// The size of the SHA-256 digest
extern const int32_t DIGEST_SIZE;

/*
 * Generates an SHA-256 Salt using a Cryptographically-Secure Pseudorandom
 * Number Generator.
 *
 * salt: Array to fill with salt bytes.
 *
 * returns: Whether the operation succeded.
 */
int32_t sha_gen_salt(uint8_t salt[SALT_SIZE]);

/*
 * Hashes the supplied message using SHA-256 algorithm, using specified salt,
 * and cycle count.
 *
 * msg: Message to create a digest of.
 * len: Length of the message.
 * salt: Salt to use when hashing.
 * cycles: Hashing cycle count.
 * result: The hashed message.
 *
 * returns: Whether the operation succeeded.
 */
int32_t sha_hash(const uint8_t *msg, size_t len, uint8_t salt[SALT_SIZE], uint16_t cycles, uint8_t result[DIGEST_SIZE]);

// Define C extern for C++
#ifdef __cplusplus
}
#endif
