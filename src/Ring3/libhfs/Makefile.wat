# Open Watcom 2.0 Makefile for HFS/2 libhfs (Ring 3 version)

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc386.exe
ASM = $(WATCOM)\binp\wasm.exe
LIB = $(WATCOM)\binp\wlib.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\Ring3\libhfs
OBJ_DIR = $(PROJ_ROOT)\bin\obj\ring3lib

# Output
NAME = libhfs
LIBFILE = $(PROJ_ROOT)\lib\libhfs32.lib

# Compiler flags
# -ml: large memory model (32-bit) - avoids relative relocation limits for DLL calls
# -bt=os2: target OS/2 (affects calling convention and relocations)
# -zc: preserve case
# -ot: optimization for time
# -d: define macros
# -i: include path
# Note: -dINCL_32 removed to avoid __watcall convention for C runtime functions
CFLAGS = -s -mf -bt=os2 -zc -ot -d__OS2__ -dIFS -DNODEVLOCKS -I$(WATCOM)\h -I$(WATCOM)\h\os2 -I$(PROJ_ROOT)\src\include

# Objects
COBJS = $(OBJ_DIR)\block.obj $(OBJ_DIR)\btree.obj $(OBJ_DIR)\data.obj $(OBJ_DIR)\file.obj $(OBJ_DIR)\hfs.obj $(OBJ_DIR)\low.obj $(OBJ_DIR)\node.obj $(OBJ_DIR)\record.obj $(OBJ_DIR)\volume.obj

all: $(LIBFILE)

$(LIBFILE): $(COBJS)
	$(LIB) $(LIBFILE) $(COBJS)

# Explicit rules
$(OBJ_DIR)\block.obj: $(SRC_DIR)\block.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\btree.obj: $(SRC_DIR)\btree.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\data.obj: $(SRC_DIR)\data.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\file.obj: $(SRC_DIR)\file.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\hfs.obj: $(SRC_DIR)\hfs.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\low.obj: $(SRC_DIR)\low.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\node.obj: $(SRC_DIR)\node.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\record.obj: $(SRC_DIR)\record.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\volume.obj: $(SRC_DIR)\volume.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(LIBFILE) 2>nul