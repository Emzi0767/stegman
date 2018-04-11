CC=gcc
LD=ld

CFLAGS=-std=c99 -Wall

ODIR=.
ONAME=stegman
OBJ=obj/

LIBS=-lm -lssl -lpng -lz
DEPS = sha256.h aes.h zlib.h steg.h defs.h
OBJS = $(OBJ)sha256.o $(OBJ)aes.o $(OBJ)zlib.o $(OBJ)steg.o $(OBJ)program.o

$(OBJ)%.o: %.c $(DEPS)
	@[ -d $(OBJ) ] || mkdir -p $(OBJ)
	@echo " [ CC ] " $@
	@$(CC) -c -o $@ $< $(CFLAGS)

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
