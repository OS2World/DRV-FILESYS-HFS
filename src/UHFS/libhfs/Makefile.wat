# Open Watcom 2.0 Makefile for HFS/2 libhfs (UHFS version)

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc.exe
LIB = $(WATCOM)\binp\wlib.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\UHFS\libhfs
OBJ_DIR = $(PROJ_ROOT)\bin\obj\uhfslib

# Output
NAME = libhfs
LIBFILE = $(PROJ_ROOT)\lib\libhfs16.lib

# Compiler flags
CFLAGS = -bt=os2 -ml -zc -3 -ot -d__OS2__ -dDEBUG -dNODEVLOCKS -dUHFS -I$(WATCOM)\h\os21x -I$(WATCOM)\h -I$(PROJ_ROOT)\src\include

# Objects
COBJS = $(OBJ_DIR)\block.obj $(OBJ_DIR)\btree.obj $(OBJ_DIR)\data.obj $(OBJ_DIR)\file.obj $(OBJ_DIR)\hfs.obj $(OBJ_DIR)\low.obj $(OBJ_DIR)\node.obj $(OBJ_DIR)\os2func.obj $(OBJ_DIR)\record.obj $(OBJ_DIR)\volume.obj

all: $(LIBFILE)

$(LIBFILE): $(COBJS)
	$(LIB) $(LIBFILE) $(COBJS)

# Explicit rules
$(OBJ_DIR)\block.obj: $(SRC_DIR)\block.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\btree.obj: $(SRC_DIR)\btree.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\data.obj: $(SRC_DIR)\data.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\file.obj: $(SRC_DIR)\file.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\hfs.obj: $(SRC_DIR)\hfs.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\low.obj: $(SRC_DIR)\low.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\node.obj: $(SRC_DIR)\node.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\os2func.obj: $(SRC_DIR)\os2func.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\record.obj: $(SRC_DIR)\record.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\volume.obj: $(SRC_DIR)\volume.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(LIBFILE) 2>nul