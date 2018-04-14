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

// The size of the AES-256 key
extern const int32_t KEY_SIZE;

// The size of the AES-256 initialization vector
extern const int32_t IV_SIZE;

/*
 * Generates an AES-256 Initialization Vector using a Cryptographically-Secure
 * Pseudorandom Number Generator.
 *
 * iv: Array to fill with IV bytes.
 *
 * returns: Whether the operation succeeded.
 */
int32_t aes_gen_iv(uint8_t iv[IV_SIZE]);

/*
 * Encrypts a specified message, using specified key and initialization vector.
 *
 * msg: Pointer to byte array, containing data to encrypt.
 * len: Length of the data to encrypt.
 * key: Key to use when encrypting.
 * iv: Initialization vector to use when encrypting.
 * result: Pointer to a pointer to byte array where encrypted data will be
 *         placed. The pointer to bytes will be initialized.
 * reslen: Pointer to integer, which will be set to resulting data length.
 *
 * returns: Whether the operation succeeded.
 */
int32_t aes_encrypt(const uint8_t *msg, uint64_t len, const uint8_t key[KEY_SIZE], const uint8_t iv[IV_SIZE], uint8_t **result, uint64_t *reslen);

/*
 * Decrypts a specified message, using specified key and initialization vector.
 *
 * msg: Pointer to byte array containing data to decrypt.
 * len: Length of the data to decrypt.
 * key: Key to use when decrypting.
 * iv: Initialization vector to use when decrypting.
 * result: Pointer to a pointer to byte array where decrypted data will be
 *         placed.
 * reslen: Pointer to integer, which will be set to resulting data length.
 *
 * returns: Whether the operation succeeded.
 */
int32_t aes_decrypt(const uint8_t *msg, uint64_t len, const uint8_t key[KEY_SIZE], const uint8_t iv[IV_SIZE], uint8_t **result, uint64_t *reslen);

// Define C extern for C++
#ifdef __cplusplus
}
#endif
