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

// Enable unicode
#define _UNICODE
#define UNICODE

// Standard types
#include <stddef.h>

// Integer types
#include <stdint.h>

// Boolean type
#include <stdbool.h>

// Char types
#include <ctype.h>

// Wide char types
#include <wchar.h>
#include <wctype.h>

// Program metadata
extern const wchar_t* const PROGRAM_NAME;
extern const wchar_t* const PROGRAM_VERSION;
extern const wchar_t* const PROGRAM_AUTHOR;
extern const wchar_t* const PROGRAM_DESCRIPTION;

// Function declarations

// Function declarations
/*
 * Prints a formatted string to standard error.
 *
 * format: Format for the outputted string.
 * args: Arguments for the string formatter.
 *
 * returns: The number of formatted items.
 */
int32_t werrorf(const wchar_t *format, ...);

/*
 * Prints information about program usage.
 *
 * progname: Program invocation.
 */
void print_usage(char *progname);

/*
 * Quits the program with specified error message and status code.
 *
 * code: Error code to quit with.
 * format: Message to quit the program with.
 * ...: Arguments to format the message.
 */
void fail(int32_t code, wchar_t *format, ...);

// Define C extern for C++
#ifdef __cplusplus
}
#endif
