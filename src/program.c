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
#include "encode.h"
#include "decode.h"

// Include standard library
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

// Constant definitions
const wchar_t *const PROGRAM_NAME        = L"Stegman";
const wchar_t *const PROGRAM_VERSION     = L"1.0.0";
const wchar_t *const PROGRAM_AUTHOR      = L"Mateusz Brawański (Emzi0767)";
const wchar_t *const PROGRAM_DESCRIPTION = L"Stegman is a small utility for safely encoding files or messages in other files using steganography.";

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

	// Check if the first argument is encode or decode
	size_t oplen = strlen(argv[1]);
	for (int i = 0; i < oplen; i++)
		argv[1][i] = tolower(argv[1][i]);
	
	// Attempt to load the PNG file
	FILE *fpng = fopen(argv[3], "rb+");
	if (!fpng)
		fail(2, L"There was an error opening '%s'\n", argv[3]);

	// Convert the password to a proper-type string
	size_t pwlen = mbcslen(argv[2]);
	wchar_t *pw = (wchar_t*)calloc(pwlen + 1, sizeof(wchar_t));
	if (!pw)
	{
		fclose(fpng);
		fail(128, L"Could not allocate memory (E_PWD_ALLOC)\n");
	}

	mbtowc(NULL, NULL, 0);
	if (!mbstowcs(pw, argv[2], pwlen))
	{
		free(pw);
		fclose(fpng);
		fail(256, L"Could not convert password (E_PWD_MBCSTOWCS)");
	}

	((wchar_t*)pw)[pwlen] = L'\0';

	if (strcmp(argv[1], "encode") == 0 && argc == 5)
	{
		// Encode the data

		// Check if the input message is a file
		bool isfile = argv[4][0] == '@';
		uint8_t *msg = NULL;
		size_t msglen = 0;
		if (isfile)
		{
			// Handle as file
			FILE *fmsg = fopen(argv[4] + 1, "rb");
			if (!fmsg)
			{
				free(pw);
				fclose(fpng);
				fail(2048, L"There was an error opening '%s'\n", argv[4] + 1);
			}

			if (fseek(fmsg, 0L, SEEK_END) != 0)
			{
				fclose(fmsg);
				free(pw);
				fclose(fpng);
				fail(4096, L"There was an error measuring the file size (E_MSG_SEEK_END)\n");
			}

			msglen = ftell(fmsg);
			if ((int64_t)msglen == -1)
			{
				fclose(fmsg);
				free(pw);
				fclose(fpng);
				fail(8192, L"There was an error measuring the file size (E_MSG_GET_POS)\n");
			}

			if (fseek(fmsg, 0L, SEEK_SET) != 0)
			{
				fclose(fmsg);
				free(pw);
				fclose(fpng);
				fail(16384, L"There was an error measuring the file size (E_MSG_SEEK_BEGIN)\n");
			}

			msg = (uint8_t*)calloc(msglen, sizeof(uint8_t));
			if (!msg)
			{
				fclose(fmsg);
				free(pw);
				fclose(fpng);
				fail(32768, L"Could not allocate memory (E_MSG_ALLOC)\n");
			}

			if (fread(msg, sizeof(uint8_t), msglen, fmsg) != msglen)
			{
				free(msg);
				fclose(fmsg);
				free(pw);
				fclose(fpng);
				fail(65536, L"Could not read message data (E_BUFFER_UNDERRUN)\n");
			}

			fclose(fmsg);
		}
		else
		{
			// Handle as unicode string
			mblen(NULL, 0);
			size_t mlen = mbcslen(argv[4]);
			msg = (uint8_t*)calloc(mlen + 1, sizeof(wchar_t));
			msglen = mlen * sizeof(wchar_t);
			if (!msg)
			{
				free(pw);
				fclose(fpng);
				fail(512, L"Could not allocate memory (E_MSG_ALLOC)\n");
			}

			mbtowc(NULL, NULL, 0);
			if (!mbstowcs((wchar_t*)msg, argv[4], mlen))
			{
				free(msg);
				free(pw);
				fclose(fpng);
				fail(1024, L"Could not convert message (E_MSG_MBCSTOWCS)\n");
			}

			((wchar_t*)msg)[mlen] = L'\0';
		}

		// Encode the data
		bool succ = encode(pw, pwlen, fpng, msg, msglen, isfile);
		if (succ)
			wprintf(L"This was a triumph! The data was successfully encoded into file '%s'!\n", argv[3]);
		else
			wprintf(L"The encoding failed :(\n");

		// Free the memory
		free(msg);
	}
	else if (strcmp(argv[1], "decode") == 0 && (argc == 4 || argc == 5))
	{
		// Decode the data
		bool isfile = false;
		uint8_t *msg = NULL;
		size_t msglen = 0;
		bool succ = decode(pw, pwlen, fpng, &msg, &msglen, &isfile);
		if (succ)
			wprintf(L"This was a triumph! The data was successfully decoded from file '%s'!\n", argv[3]);
		else
			wprintf(L"The decoding failed :(\n");

		if (argc == 5)
		{
			// Save the file
			FILE *fmsg = fopen(argv[4], "wb");
			if (!fmsg)
			{
				free(msg);
				fail(2048, L"There was an error opening '%s'\n", argv[4]);
			}

			if (fwrite(msg, sizeof(uint8_t), msglen, fmsg) != msglen)
			{
				free(msg);
				fclose(fmsg);
				fail(4096, L"There was an error writing to '%s'\n", argv[4]);
			}

			fclose(fmsg);
		}
		else if (isfile && argc == 4)
		{
			werrorf(L"The message was decoded successfully, however the source was a file. To save it, you need to specify an output file when launching the program.\n");
		}
		else
		{
			wprintf(L"Decoded message:\n\n%ls\n", (wchar_t*)msg);
		}

		// Free the memory
		free(msg);
	}
	else
	{
		print_usage(argv[0]);
		return 1;
	}

	free(pw);
	fclose(fpng);

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
	werrorf(L"%ls v%ls by %ls\n%ls\n\nUsage:\n", PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_AUTHOR, PROGRAM_DESCRIPTION);
	werrorf(L"In order to use %ls, you need to specify operation mode. The program has 2 modes: encode, decode. Described below are arguments for each available mode.\n\n", PROGRAM_NAME);
	werrorf(L"%s encode <password> <target file> <message>\n%s encode <password> <target file> @<source file>\npassword       The password to secure your data before encoding.\ntarget file    The file in which the data will be encoded.\nmessage        Text message to encode in the file.\nsource file    File to encode in the file.\n\n", progname, progname);
	werrorf(L"%s decode <password> <source file> [target file]\npassword       The password used to secure your encoded data.\nsource file    The file in which the data was encoded.\ntarget file    The file in which the decoded data will be placed.\n\n", progname);
	werrorf(L"When decoding, and the encoded data comes from a file, you need to specify the target file.\n");

#ifdef __BUILDINFO__
	werrorf(L"\nBuild info:\nTimestamp:        %ls\nCommit:           %ls\nPath:             %ls\nMachine:          %ls\nUser:             %ls\nCompiler:         %ls\nHost:             %ls\n", __TIMESTAMP_ISO__, __GIT_COMMIT__, __WORKDIR__, __MACHINE__, __USER__, __COMPILER__, __HOST__);
#endif // __BUILDINFO__
}

void fail(int32_t code, wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);

	vfwprintf(stderr, format, args);

	va_end(args);
	exit(code);
}

size_t mbcslen(char *str)
{
	mblen(NULL, 0);

	size_t len = 0, tmp = 0;
	char *x = str;
	do
	{
		tmp = mblen(x, MB_CUR_MAX);
		
		len++;
		x += tmp;
	}
	while (tmp > 0 && *x != '\0');

	return len;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
