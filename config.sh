#!/usr/bin/env bash

# Check if Makefile is already prepared
if [ -f "Makefile" ]
then
	echo -e "\e[1m\e[93mWARNING: \e[0mA Makefile already exists, it will be deleted!"
	rm "Makefile"
fi

# Copy the input makefile to target location
echo "Generating Makefile..."
cp "Makefile.in" "Makefile.tmp"
cat <<EOF > config.c
#include <stdio.h>
int main(void)
{
	printf("Hello world!\n");
	return 0;
}
EOF

# Detect a compiler
echo -n "Detecting compiler... "
if [ -z "$CC" ]
then
	CC=cc
fi
echo "$CC"

# See if compiler is working
echo -n "Checking if $CC is a working C99 compiler... "
if ! "$CC" -o "config.out" "config.c" -std=c99 -Wall -g &>/dev/null
then
	echo "no"
	echo -e "\e[1m\e[31mERROR: \e[0m$CC is not a working compiler"
	rm "Makefile.tmp"
	rm "config.c"
	exit 1
fi
echo "yes"
rm "config.out"

# See if there are any extra CFLAGS
echo -n "Checking for any extra CFLAGS... "
if [ ! -z "$CFLAGS" ]
then
	if ! "$CC" -o "config.out" "config.c" -std=c99 -Wall -g $CFLAGS -lm &>/dev/null
	then
		echo "invalid"
		echo -e "\e[1m\e[31mERROR: \e[0m$CFLAGS CFLAGS are not valid!"
		rm "Makefile.tmp"
		rm "config.c"
		exit 1
	fi
	echo "$CFLAGS"
	rm "config.out"
else
	echo "none"
fi

# See if there are any extra LDFLAGS
echo -n "Checking for any extra LDFLAGS... "
if [ ! -z "$LDFLAGS" ]
then
	if ! "$CC" -o "config.out" "config.c" -std=c99 -Wall -g -lm $LDFLAGS &>/dev/null
	then
		echo "invalid"
		echo -e "\e[1m\e[31mERROR: \e[0m$LDFLAGS LDFLAGS are not valid!"
		rm "Makefile.tmp"
		rm "config.c"
		exit 1
	fi
	echo "$LDFLAGS"
	rm "config.out"
else
	echo "none"
fi

# Check if ZLib development files are available
echo -n "Checking for zlib... "
cat <<EOF > config.c
#include <stdio.h>
#include <zlib.h>
int main(void)
{
	printf("Hello world!\n");
	return 0;
}
EOF
if ! "$CC" -o "config.out" "config.c" -std=c99 -Wall -g $CFLAGS -lm -lz $LDFLAGS &>/dev/null
then
	echo "not present"
	echo -e "\e[1m\e[31mERROR: \e[0mZLib development files are not present in the system!"
	rm "Makefile.tmp"
	rm "config.c"
	exit 1
fi
echo "present"
rm "config.out"

# Check if OpenSSL development files are available
echo -n "Checking for openssl... "
cat <<EOF > config.c
#include <stdio.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
int main(void)
{
	printf("Hello world!\n");
	return 0;
}
EOF
if ! "$CC" -o "config.out" "config.c" -std=c99 -Wall -g $CFLAGS -lm -lcrypto -lssl $LDFLAGS &>/dev/null
then
	echo "not present"
	echo -e "\e[1m\e[31mERROR: \e[0mOpenSSL development files are not present in the system!"
	rm "Makefile.tmp"
	rm "config.c"
	exit 1
fi
echo "present"
rm "config.out"

# Check if libpng development files are available
echo -n "Checking for png... "
cat <<EOF > config.c
#include <stdio.h>
#include <png.h>
int main(void)
{
	printf("Hello world!\n");
	return 0;
}
EOF
if ! "$CC" -o "config.out" "config.c" -std=c99 -Wall -g $CFLAGS -lm -lpng $LDFLAGS &>/dev/null
then
	echo "not present"
	echo -e "\e[1m\e[31mERROR: \e[0mlibpng development files are not present in the system!"
	rm "Makefile.tmp"
	rm "config.c"
	exit 1
fi
echo "present"
rm "config.out"

# Transform the Makefile appropriately
echo -n "Saving changes... "
echo -e "CC=\"$CC\"\n\nCFLAGS=$CFLAGS\nLDFLAGS=$LDFLAGS\n" > Makefile
cat "Makefile.tmp" >> Makefile
echo "done"

# Clean up
echo -n "Cleaning up... "
rm "config.c"
rm "Makefile.tmp"
echo "done"

echo -e "\nYou can now run \e[96mmake\e[0m to build the project"
