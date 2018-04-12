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

// Include relevant headers
#include "defs.h"

// Include standard library
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

// Constant definitions
const wchar_t* const PROGRAM_NAME        = L"Stegman";
const wchar_t* const PROGRAM_VERSION     = L"1.0.0";
const wchar_t* const PROGRAM_AUTHOR      = L"Mateusz Brawański (Emzi0767)";
const wchar_t* const PROGRAM_DESCRIPTION = L"Stegman is a small utility for safely encoding files or messages in other files using steganography.";

// Function declarations
/*
 * Prints a formatted string to standard error.
 *
 * format: Format for the outputted string.
 * args: Arguments for the string formatter.
 *
 * returns: The number of formatted items.
 */
int32_t werrorf(const wchar_t* format, ...);

/*
 * Prints information about program usage.
 *
 * progname: Program invocation.
 */
void print_usage(char* progname);

// Entry point
int32_t main(int argc, char** argv)
{
	// Set locale appropriately
	setlocale(LC_ALL, "");

	// Check if there's enough arguments supplied
	if (argc < 3 || argc > 5)
	{
		print_usage(argv[0]);
		return 1;
	}

	// End of program
	return 0;
}

// Function definitions
int32_t werrorf(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	int32_t res = vfwprintf(stderr, format, args);

	va_end(args);

	return res;
}

void print_usage(char* progname)
{
	werrorf(L"%ls v%ls by %ls\n%ls\nUsage:\n\n", PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_AUTHOR, PROGRAM_DESCRIPTION);
	werrorf(L"In order to use %ls, you need to specify operation mode. The program has 2\nmodes: encode, decode. Described below are arguments for each available mode.\n\n", PROGRAM_NAME);
	werrorf(L"%s encode <password> <target file> <message>\n%s encode <password> <target file> @<source file>\npassword       The password to secure your data before encoding.\ntarget file    The file in which the data will be encoded.\nmessage        Text message to encode in the file.\nsource file    File to encode in the file.\n\n", progname, progname);
	werrorf(L"%s decode <password> <source file> [target file]\npassword       The password used to secure your encoded data.\nsource file    The file in which the data was encoded.\ntarget file    The file in which the decoded data will be placed.\n\n", progname);
	werrorf(L"When decoding, and the encoded data comes from a file, you need to specify the target file.\n");
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
