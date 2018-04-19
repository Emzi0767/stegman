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

// Appropriate header
#include "defs.h"
#include "png.h"

// Standard library
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <png.h>

// Function definitions
int32_t png_load_pixels(FILE *src, uint8_t **tgt, size_t *tgtlen, PngImageInfo *imginfo)
{
    uint8_t header[8];
    fread(header, sizeof(uint8_t), 8, src);

    if (png_sig_cmp(header, 0, 8))
        return 1;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop png_inf = NULL;

    if (!png_ptr)
        return 2;

    png_inf = png_create_info_struct(png_ptr);
    if (!png_inf)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 8;
    }
    
    png_init_io(png_ptr, src);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, png_inf);

    imginfo->width = png_get_image_width(png_ptr, png_inf);
    imginfo->height = png_get_image_height(png_ptr, png_inf);

    int32_t bits = png_get_bit_depth(png_ptr, png_inf);
    int32_t ctpe = png_get_color_type(png_ptr, png_inf);

    if (bits != 8)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 16;
    }

    if (ctpe != PNG_COLOR_TYPE_RGB && ctpe != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 32;
    }

    imginfo->bit_depth = bits * (ctpe == PNG_COLOR_TYPE_RGB_ALPHA ? 4 : 3);

    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, png_inf);

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 64;
    }

    int32_t rowsize = png_get_rowbytes(png_ptr, png_inf);
    uint8_t **rows = (uint8_t**)calloc(imginfo->height, sizeof(uint8_t*));
    for (int32_t i = 0; i < imginfo->height; i++)
        rows[i] = (uint8_t*)calloc(rowsize, sizeof(uint8_t));

    png_read_image(png_ptr, rows);

    *tgtlen = imginfo->width * imginfo->height * (imginfo->bit_depth / 8);
    *tgt = (uint8_t*)calloc(*tgtlen, sizeof(uint8_t*));

    uint8_t* tgtptr = *tgt;
    for (int32_t i = 0; i < imginfo->height; i++)
    {
        memcpy(tgtptr, rows[i], rowsize * sizeof(uint8_t));
        tgtptr += rowsize;
        free(rows[i]);
    }
    free(rows);

    return 0;
}

int32_t png_save_pixels(const uint8_t *src, size_t srclen, const PngImageInfo *imginfo, FILE *tgt)
{
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop png_inf = NULL;

    if (!png_ptr)
        return 1;

    png_inf = png_create_info_struct(png_ptr);
    if (!png_inf)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return 2;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return 4;
    }

    png_init_io(png_ptr, tgt);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return 8;
    }

    int32_t colourtype = imginfo->bit_depth / 8;
    colourtype = colourtype == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;
    png_set_IHDR(png_ptr, png_inf, imginfo->width, imginfo->height, 8, colourtype, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, png_inf);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return 16;
    }

    int32_t rowsize = png_get_rowbytes(png_ptr, png_inf);
    uint8_t **rows = (uint8_t**)calloc(imginfo->height, sizeof(uint8_t*));
    const uint8_t *pxdat = src;
    for (int32_t i = 0; i < imginfo->height; i++)
    {
        rows[i] = (uint8_t*)calloc(rowsize, sizeof(uint8_t));
        memcpy(rows[i], pxdat, rowsize * sizeof(uint8_t));
        pxdat += rowsize;
    }

    png_write_image(png_ptr, rows);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return 32;
    }

    png_write_end(png_ptr, NULL);

    for (int32_t i = 0; i < imginfo->height; i++)
        free(rows[i]);
    free(rows);
    return 0;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
