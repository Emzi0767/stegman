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
 * Metadata about the loaded image. This is basic information about the loaded
 * pixel data.
 */
typedef struct PngImageInfo
{
	/**
	 * Width of the image, in pixels.
	 */
	int32_t width;

	/**
	 * Height of the image, in pixels.
	 */
	int32_t height;

	/**
	 * Colour depth of the picture, in bits. To get the number of bytes per 
	 * pixel, divide this number by 8.
	 */
	uint8_t bit_depth; 
} PngImageInfo;

/**
 * Loads pixels from a supplied PNG image.
 *
 * src: Bytes of the image file.
 * srclen: Length of the source file.
 * tgt: Pointer to target bytes. This pointer will be initialized.
 * tgtlen: Pointer to length of resulting data.
 * imginfo: Information about the image.
 *
 * \return 0 if the operation was successful, an error code otherwise.
 */
int32_t png_load_pixels(const uint8_t *src, size_t srclen, uint8_t **tgt, size_t *tgtlen, PngImageInfo *imginfo);

/**
 * Writes supplied pixels to a PNG file.
 *
 * src: Pixels to write.
 * srclen: Length of the pixel array.
 * imginfo: Information about the pixels.
 * tgt: Pointer to target PNG bytes. This pointer will be initialized.
 * tgtlen: Pointer to length of resulting data.
 *
 * \return 0 if the operation was successful, an error code otherwise.
 */
int32_t png_save_pixels(const uint8_t *src, size_t srclen, const PngImageInfo imginfo, uint8_t **tgt, size_t *tgtlen);

// Define C extern for C++
#ifdef __cplusplus
}
#endif
