CC=gcc
LD=ld

CFLAGS=-std=c99 -Wall
TIMESTAMP=$(shell date -u +'\"%Y-%m-%dT%H:%M:%SZ\"')
COMMIT=\"$(shell git rev-parse HEAD)\"

ODIR=.
ONAME=stegman
OBJ=obj/

LIBS=-lm -lssl -lpng -lz
DEPS = sha256.h aes.h zlib.h steg.h png.h defs.h
OBJS = $(OBJ)sha256.o $(OBJ)aes.o $(OBJ)zlib.o $(OBJ)steg.o $(OBJ)png.o $(OBJ)program.o

$(OBJ)%.o: %.c $(DEPS)
	@[ -d $(OBJ) ] || mkdir -p $(OBJ)
	@echo " [ CC ] " $@
	@$(CC) -c -o $@ $< $(CFLAGS) -finput-charset=UTF-8 -D__TIMESTAMP_ISO__=$(TIMESTAMP) -D__GIT_COMMIT__=$(COMMIT)

$(ODIR)/$(ONAME): $(OBJS)
	@[ -d $(ODIR) ] || mkdir -p $(ODIR)
	@echo " [ LD ] " $@
	@$(CC) -fuse-ld=$(LD) $(LIBS) $^ -o $@
	@echo " [ ST ] " $@
	@strip $@

.PHONY: clean

clean: $(ODIR)/$(ONAME)
	@echo " [ RM ] " $(OBJ)
	@rm -rf $(OBJ)
	@echo " [ RM ] " $^
	@rm $^
