# Open Watcom 2.0 Makefile for HFS/2 Prepare Utility

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc386.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\util\Prepare
OBJ_DIR = $(PROJ_ROOT)\bin\obj\prepare

# Output
NAME = prepare
EXE = $(PROJ_ROOT)\bin\$(NAME).exe

# Compiler flags
CFLAGS = -s -mf -zc -3 -ot -d__OS2__ -I$(WATCOM)\h -I$(WATCOM)\h\os2 -I$(PROJ_ROOT)\src\include

# Linker flags - 32-bit OS/2 application
LFLAGS = format os2 lx

# Objects
COBJS = $(OBJ_DIR)\prepare.obj $(OBJ_DIR)\names.obj $(OBJ_DIR)\boot.obj

all: $(EXE)
	@echo Prepare utility built. > all

$(EXE): $(COBJS)
	$(LINK) @<<
format os2 lx
name $(EXE)
option description '@#OS2World:0.20#@HFS/2 - PREPARE utility'
option map=$(OBJ_DIR)\$(NAME).map
option symfile=$(OBJ_DIR)\$(NAME).sym
file $(OBJ_DIR)\prepare.obj
file $(OBJ_DIR)\names.obj
file $(OBJ_DIR)\boot.obj
<<

# Explicit rules
$(OBJ_DIR)\prepare.obj: $(SRC_DIR)\prepare.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\names.obj: $(SRC_DIR)\names.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\boot.obj: $(SRC_DIR)\boot.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(OBJ_DIR)\*.sym 2>nul
	del $(EXE) 2>nul