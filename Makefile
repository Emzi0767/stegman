CC=gcc
LD=ld

CFLAGS=
LDFLAGS=

TIMESTAMP="$(shell date -u +'L\"%Y-%m-%dT%H:%M:%SZ\"')"
COMMIT="L\"$(shell git rev-parse HEAD)\""
WORKDIR="L\"$(shell pwd)\""
MACHINE="L\"$(shell hostname)\""
USER="L\"$(shell whoami)\""
COMPILER="L\"$(shell $(CC) --version | head -n1)\""
HOST="L\"$(shell uname -a)\""

ODIR=.
ONAME=stegman
OBJ=obj/

LIBS=-lm -lssl -lcrypto -lpng -lz
DEPS = sha256.h aes.h zlib.h steg.h png.h defs.h encode.h decode.h
OBJS = $(OBJ)sha256.o $(OBJ)aes.o $(OBJ)zlib.o $(OBJ)steg.o $(OBJ)png.o $(OBJ)encode.o $(OBJ)decode.o $(OBJ)program.o

$(OBJ)%.o: %.c $(DEPS)
	@[ -d $(OBJ) ] || mkdir -p $(OBJ)
	@echo " [ CC ] " $@
	@$(CC) -std=c99 -Wall -c -o $@ $< $(CFLAGS) -finput-charset=UTF-8 -D__BUILDINFO__ -D__TIMESTAMP_ISO__=$(TIMESTAMP) -D__GIT_COMMIT__=$(COMMIT) -D__WORKDIR__=$(WORKDIR) -D__MACHINE__=$(MACHINE) -D__USER__=$(USER) -D__COMPILER__=$(COMPILER) -D__HOST__=$(HOST)

$(ODIR)/$(ONAME): $(OBJS)
	@[ -d $(ODIR) ] || mkdir -p $(ODIR)
	@echo " [ LD ] " $@
	@$(CC) -fuse-ld=$(LD) $(LIBS) $^ -o $@ $(LDFLAGS)
	@echo " [ ST ] " $@
	@strip $@

.PHONY: clean

clean: $(ODIR)/$(ONAME)
	@echo " [ RM ] " $(OBJ)
	@rm -rf $(OBJ)
	@echo " [ RM ] " $^
	@rm $^

$(ODIR)/$(ONAME)-dbg: $(OBJS)
	@[ -d $(ODIR) ] || mkdir -p $(ODIR)
	@echo " [ LD ] " $@
	@$(CC) -fuse-ld=$(LD) $(LIBS) $^ -o $@ $(LDFLAGS)

debug: $(ODIR)/$(ONAME)-dbg

clean-debug: $(ODIR)/$(ONAME)-dbg
	@echo " [ RM ] " $(OBJ)
	@rm -rf $(OBJ)
	@echo " [ RM ] " $^
	@rm $^

docs: $(OBJS)
	@echo " [DOCS]  docs/"
	@doxygen Doxyfile

clean-docs: docs
	@echo " [ RM ]  docs/"
	@rm -rf docs
