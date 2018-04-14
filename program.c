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
#include "aes.h"
#include "sha256.h"
#include "zlib.h"
#include "png.h"
#include "steg.h"

// Include standard library
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// OpenSSL
#include <openssl/err.h>

// Constant definitions
const wchar_t *const PROGRAM_NAME        = L"Stegman";
const wchar_t *const PROGRAM_VERSION     = L"1.0.0";
const wchar_t *const PROGRAM_AUTHOR      = L"Mateusz Brawański (Emzi0767)";
const wchar_t *const PROGRAM_DESCRIPTION = L"Stegman is a small utility for safely encoding files or messages in other files using steganography.";

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

/*
 * Encodes data.
 *
 * password: Password to encrypt the data with.
 * passlen: Length of the password.
 * file: Data of the file to encode the data into.
 * filelen: Length of the file data to encode into.
 * message: Message data to encode.
 * msglen: Length of the message to encode.
 * isfile: Whether the message was a file.
 *
 * returns: Whether the operation was successful.
 */
bool encode(const wchar_t *password, size_t passlen, uint8_t *file, size_t filelen, const uint8_t *message, size_t msglen, bool isfile);

/*
 * Decodes data.
 *
 * password: Password to decrypt the data with.
 * passlen: Length of the password.
 * file: Data of the file to decode the data from.
 * filelen: Length of the file to decode data from.
 * message: Pointer to message bytes. Underlying pointer will be initialized.
 * msglen: Length of the resulting message.
 * isfile: Whether the message is a file.
 *
 * returns: Whether the operation was successful.
 */
bool decode(const wchar_t *password, size_t passlen, const uint8_t *file, size_t filelen, uint8_t **message, size_t *msglen, bool *isfile);

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

	if (strcmp(argv[1], "encode") == 0 && argc == 5)
	{
		// Encode the data

		// Attempt to load the PNG file
		FILE *fpng = fopen(argv[3], "rb");
		if (!fpng)
			fail(2, L"There was an error opening '%s'\n", argv[3]);

		// Get its size
		if (fseek(fpng, 0L, SEEK_END) != 0)
		{
			fclose(fpng);
			fail(4, L"There was an error measuring the file size (E_ERR_SEEK_END)\n");
		}

		int64_t fsize = ftell(fpng);
		if (fsize == -1L)
		{
			fclose(fpng);
			fail(8, L"There was an error measuring the file size (E_ERR_GET_POS)\n");
		}

		if (fseek(fpng, 0L, SEEK_SET) != 0)
		{
			fclose(fpng);
			fail(16, L"There was an error measuring the file size (E_ERR_SEEK_BEGIN)\n");
		}

		// Allocate the PNG buffer
		uint8_t *fdata = (uint8_t*)calloc(fsize, sizeof(uint8_t));
		if (!fdata)
		{
			fclose(fpng);
			fail(32, L"Could not allocate memory (E_PNG_ALLOC)\n");
		}

		// Read the data into the PNG buffer
		if (fread(fdata, sizeof(uint8_t), (size_t)fsize, fpng) != (size_t)fsize)
		{
			free(fdata);
			fclose(fpng);
			fail(64, L"Could not read PNG data (E_BUFFER_UNDERRUN)\n");
		}

		// Close the PNG file, we don't need it anymore
		fclose(fpng);

		// Convert the password to a proper-type string
		size_t pwlen = mblen(argv[2], MB_CUR_MAX) + sizeof(wchar_t);
		wchar_t *pw = (wchar_t*)calloc(pwlen, sizeof(wchar_t));
		if (!pw)
		{
			free(fdata);
			fail(128, L"Could not allocate memory (E_PWD_ALLOC)\n");
		}

		if (!mbstowcs(pw, argv[2], pwlen))
		{
			free(pw);
			free(fdata);
			fail(256, L"Could not convert password (E_PWD_MBCSTOWCS)");
		}

		// Check if the input message is a file
		bool isfile = argv[4][0] == '@';
		uint8_t *msg = NULL;
		size_t msglen;
		if (isfile)
		{
			// Handle as file
			FILE *fmsg = fopen(argv[4] + 1, "rb");
			if (!fmsg)
			{
				free(pw);
				free(fdata);
				fail(2048, L"There was an error opening '%s'\n", argv[4] + 1);
			}

			if (fseek(fmsg, 0L, SEEK_END) != 0)
			{
				fclose(fmsg);
				free(pw);
				free(fdata);
				fail(4096, L"There was an error measuring the file size (E_MSG_SEEK_END)\n");
			}

			msglen = ftell(fpng);
			if ((int64_t)msglen == -1)
			{
				fclose(fmsg);
				free(pw);
				free(fdata);
				fail(8192, L"There was an error measuring the file size (E_MSG_GET_POS)\n");
			}

			if (fseek(fmsg, 0L, SEEK_SET) != 0)
			{
				fclose(fmsg);
				free(pw);
				free(fdata);
				fail(16384, L"There was an error measuring the file size (E_MSG_SEEK_BEGIN)\n");
			}

			msg = (uint8_t*)calloc(msglen, sizeof(uint8_t));
			if (!msg)
			{
				fclose(fmsg);
				free(pw);
				free(fdata);
				fail(32768, L"Could not allocate memory (E_MSG_ALLOC)\n");
			}

			if (fread(msg, sizeof(uint8_t), msglen, fmsg) != msglen)
			{
				free(msg);
				fclose(fmsg);
				free(pw);
				free(fdata);
				fail(65536, L"Could not read message data (E_BUFFER_UNDERRUN)\n");
			}

			fclose(fmsg);
		}
		else
		{
			// Handle as unicode string
			size_t mlen = mblen(argv[4], MB_CUR_MAX) + sizeof(wchar_t);
			msg = (uint8_t*)calloc(mlen, sizeof(wchar_t));
			msglen = mlen * sizeof(wchar_t);
			if (!msg)
			{
				free(pw);
				free(fdata);
				fail(512, L"Could not allocate memory (E_MSG_ALLOC)\n");
			}

			if (!mbstowcs((wchar_t*)msg, argv[4], mlen))
			{
				free(msg);
				free(pw);
				free(fdata);
				fail(1024, L"Could not convert message (E_MSG_MBCSTOWCS)\n");
			}
		}

		// Encode the data
		bool succ = encode(pw, pwlen, fdata, (size_t)fsize, msg, msglen, isfile);
		if (succ)
			wprintf(L"This was a triumph! The data was successfully encoded into file '%s'!\n", argv[3]);
		else
			wprintf(L"The encoding failed :(\n");

		// Free the memory
		free(msg);
		free(pw);
		free(fdata);
	}
	else if (strcmp(argv[1], "decode") == 0 && (argc == 3 || argc == 4))
	{
		// Decode the data
	}
	else
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
	werrorf(L"%ls v%ls by %ls\n%ls\n\nUsage:\n", PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_AUTHOR, PROGRAM_DESCRIPTION);
	werrorf(L"In order to use %ls, you need to specify operation mode. The program has 2 modes: encode, decode. Described below are arguments for each available mode.\n\n", PROGRAM_NAME);
	werrorf(L"%s encode <password> <target file> <message>\n%s encode <password> <target file> @<source file>\npassword       The password to secure your data before encoding.\ntarget file    The file in which the data will be encoded.\nmessage        Text message to encode in the file.\nsource file    File to encode in the file.\n\n", progname, progname);
	werrorf(L"%s decode <password> <source file> [target file]\npassword       The password used to secure your encoded data.\nsource file    The file in which the data was encoded.\ntarget file    The file in which the decoded data will be placed.\n\n", progname);
	werrorf(L"When decoding, and the encoded data comes from a file, you need to specify the target file.\n");

#ifdef __BUILDINFO__
	werrorf(L"\nBuild info:\nTimestamp:   %ls\nCommit:      %ls\nPath:        %ls\nMachine:     %ls\nUser:        %ls\nCompiler:    %ls\nHost:        %ls\n", __TIMESTAMP_ISO__, __GIT_COMMIT__, __WORKDIR__, __MACHINE__, __USER__, __COMPILER__, __HOST__);
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

bool encode(const wchar_t *password, size_t passlen, uint8_t *file, size_t filelen, const uint8_t *message, size_t msglen, bool isfile)
{
	uint8_t salt[SALT_SIZE], iv[IV_SIZE], key[KEY_SIZE];

	// Generate salt
	int32_t res = sha_gen_salt(salt);
	if (!res)
	{
		werrorf(L"Error generating salt (%lu). Refer to OpenSSL docs for RAND_bytes for more details.\n", ERR_get_error());
		return false;
	}

	// Generate IV
	res = aes_gen_iv(iv);
	if (!res)
	{
		werrorf(L"Error generating IV (%lu). Refer to OpenSSL docs for RAND_bytes for more details.\n", ERR_get_error());
		return false;
	}

	// Generate hash cycle count
	srand(time(NULL));
	uint16_t hc = (uint16_t)(rand() % 32768 + 32767);

	// Create the AES key by hashing the password using SHA-256
	res = sha_hash((uint8_t*)password, passlen * sizeof(wchar_t), salt, hc, key);
	if (!res)
	{
		werrorf(L"Error generating AES key (%lu). Refer to OpenSSL docs for SHA256 for more details.\n", ERR_get_error());
		return false;
	}

	// Compress the data
	uint8_t *data = NULL;
	uint64_t datalen = 0;
	res = zlib_compress(message, msglen, &data, &datalen);
	if (res)
	{
		werrorf(L"Error compressing data (%d). Refer to ZLib manual for details.\n", res);
		return false;
	}

	// Free memory
	free(data);

	// Return success
	return true;
}

// Define C extern for C++
#ifdef __cplusplus
}
#endif
