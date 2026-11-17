# Open Watcom 2.0 Makefile for HFS/2 Ring 3 EXE

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc386.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe
LIB = $(WATCOM)\binp\wlib.exe
MAKE = $(WATCOM)\binp\wmake.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\Ring3
LIBHFS_DIR = $(SRC_DIR)\libhfs

BIN_DIR = $(PROJ_ROOT)\bin
OBJ_DIR = $(BIN_DIR)\obj\ring3

# Output
NAME = hfs
EXE = $(BIN_DIR)\$(NAME).exe
LIBFILE = $(PROJ_ROOT)\lib\libhfs32.lib
DEF = $(SRC_DIR)\$(NAME).def

# Compiler flags
# -ml: large memory model (32-bit) - avoids relative relocation limits for DLL calls
# -bt=os2: target OS/2 (affects calling convention and relocations)
# -zc: preserve case
# -ot: optimization for time
# -d: define macros
# -i: include path
# Note: -dINCL_32 removed to avoid __watcall convention for C runtime functions
CFLAGS = -s -mf -zp1 -bt=os2 -zc -ot -d__OS2__ -dIFS -DNODEVLOCKS -I$(WATCOM)\h -I$(WATCOM)\h\os2 -I$(PROJ_ROOT) -I$(PROJ_ROOT)\src\include -I$(LIBHFS_DIR) -I$(PROJ_ROOT)\src\Ring0

# Linker flags - console application
LFLAGS = format os2 pm file $(EXE) name $(NAME) option map=$(OBJ_DIR)\$(NAME).map option symfile=$(OBJ_DIR)\$(NAME).sym library $(LIBFILE) library $(WATCOM)\lib386\os2\os2386.lib

# Objects
COBJS = $(OBJ_DIR)\attr.obj $(OBJ_DIR)\cache.obj $(OBJ_DIR)\colour.obj $(OBJ_DIR)\dir.obj $(OBJ_DIR)\dirsearch.obj $(OBJ_DIR)\diskfunc.obj $(OBJ_DIR)\ea.obj $(OBJ_DIR)\error.obj $(OBJ_DIR)\file.obj $(OBJ_DIR)\fsctl.obj $(OBJ_DIR)\icon.obj $(OBJ_DIR)\log.obj $(OBJ_DIR)\logger.obj $(OBJ_DIR)\misc.obj $(OBJ_DIR)\names.obj $(OBJ_DIR)\r3comm.obj $(OBJ_DIR)\resource.obj $(OBJ_DIR)\times.obj $(OBJ_DIR)\userfunc.obj $(OBJ_DIR)\vol.obj

all: $(LIBFILE) $(EXE)

$(LIBFILE):
	$(MAKE) -a -z -f $(LIBHFS_DIR)\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)

$(EXE): $(COBJS) $(LIBFILE) $(DEF)
	$(LINK) @<<
format os2 lx
name $(EXE)
option stack=256k
option description '@#OS2World:0.20#@HFS/2 - Control program for HFS.IFS'
option map=$(OBJ_DIR)\$(NAME).map
option symfile=$(OBJ_DIR)\$(NAME).sym
option nodefaultlib
library $(LIBFILE)
library $(WATCOM)\lib386\os2\clib3r.lib
library $(WATCOM)\lib386\os2\os2386.lib
file $(OBJ_DIR)\attr.obj
file $(OBJ_DIR)\cache.obj
file $(OBJ_DIR)\colour.obj
file $(OBJ_DIR)\dir.obj
file $(OBJ_DIR)\dirsearch.obj
file $(OBJ_DIR)\diskfunc.obj
file $(OBJ_DIR)\ea.obj
file $(OBJ_DIR)\error.obj
file $(OBJ_DIR)\file.obj
file $(OBJ_DIR)\fsctl.obj
file $(OBJ_DIR)\icon.obj
file $(OBJ_DIR)\log.obj
file $(OBJ_DIR)\logger.obj
file $(OBJ_DIR)\misc.obj
file $(OBJ_DIR)\names.obj
file $(OBJ_DIR)\r3comm.obj
file $(OBJ_DIR)\resource.obj
file $(OBJ_DIR)\times.obj
file $(OBJ_DIR)\userfunc.obj
file $(OBJ_DIR)\vol.obj
<<

# Explicit rules for each object file
$(OBJ_DIR)\attr.obj: $(SRC_DIR)\attr.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\cache.obj: $(SRC_DIR)\cache.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\colour.obj: $(SRC_DIR)\colour.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\dir.obj: $(SRC_DIR)\dir.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\dirsearch.obj: $(SRC_DIR)\dirsearch.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\diskfunc.obj: $(SRC_DIR)\diskfunc.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\ea.obj: $(SRC_DIR)\ea.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\error.obj: $(SRC_DIR)\error.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\file.obj: $(SRC_DIR)\file.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\fsctl.obj: $(SRC_DIR)\fsctl.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\icon.obj: $(SRC_DIR)\icon.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\log.obj: $(SRC_DIR)\log.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\logger.obj: $(SRC_DIR)\logger.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\misc.obj: $(SRC_DIR)\misc.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\names.obj: $(SRC_DIR)\names.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\r3comm.obj: $(SRC_DIR)\r3comm.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\resource.obj: $(SRC_DIR)\resource.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\times.obj: $(SRC_DIR)\times.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\userfunc.obj: $(SRC_DIR)\userfunc.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\vol.obj: $(SRC_DIR)\vol.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(OBJ_DIR)\*.sym 2>nul
	del $(EXE) 2>nul
	del $(LIBFILE) 2>nul
	$(MAKE) -f $(LIBHFS_DIR)\Makefile.wat clean