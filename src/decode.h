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

/**
 * Decodes data from a picture. This method will read data from the supplied 
 * PNG image's pixels.
 *
 * \param password Password to decrypt the data with.
 * \param passlen Length of the password.
 * \param file Data of the file to decode the data from. This should contain 
 *             raw data of the PNG file.
 * \param filelen Length of the file to decode data from.
 * \param message Pointer to message bytes. Underlying pointer will be 
 *                initialized.
 * \param msglen Length of the resulting message.
 * \param isfile Whether the message is a file.
 *
 * \return Whether the operation was successful.
 */
bool decode(const wchar_t *password, size_t passlen, const uint8_t *file, size_t filelen, uint8_t **message, size_t *msglen, bool *isfile);

// Define C extern for C++
#ifdef __cplusplus
}
#endif