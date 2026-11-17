# Open Watcom 2.0 Makefile for HFS/2 Utilities

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc386.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe
MAKE = $(WATCOM)\binp\wmake.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\util
PREPARE_DIR = $(SRC_DIR)\Prepare
BIN_DIR = $(PROJ_ROOT)\bin
OBJ_DIR = $(BIN_DIR)\obj\util

# Outputs
FSKILL = $(BIN_DIR)\fskill.exe
SYNC = $(BIN_DIR)\sync.exe
READRES = $(BIN_DIR)\readres.exe
BINDISK = $(BIN_DIR)\bindisk.exe
PREPARE_EXE = $(BIN_DIR)\prepare.exe

# Compiler flags
CFLAGS = -s -mf -zc -3 -ot -d__OS2__ -I$(WATCOM)\h -I$(WATCOM)\h\os2 -I$(PROJ_ROOT)\src\include

# Linker flags - 32-bit OS/2 applications
LFLAGS = format os2 lx

# Objects
FSKILL_OBJS = $(OBJ_DIR)\fskill.obj
SYNC_OBJS = $(OBJ_DIR)\sync.obj
READRES_OBJS = $(OBJ_DIR)\readres.obj
BINDISK_OBJS = $(OBJ_DIR)\bindisk.obj

all: $(FSKILL) $(SYNC) $(READRES) $(BINDISK) $(PREPARE_EXE)
	@echo HFS utilities built. > all

$(FSKILL): $(FSKILL_OBJS)
	$(LINK) @<<
format os2 lx
name $(FSKILL)
option description '@#OS2World:0.20#@HFS/2 - FSKILL utility (stops the control program)'
option map=$(OBJ_DIR)\fskill.map
option symfile=$(OBJ_DIR)\fskill.sym
file $(FSKILL_OBJS)
<<

$(SYNC): $(SYNC_OBJS)
	$(LINK) @<<
format os2 lx
name $(SYNC)
option description '@#OS2World:0.20#@HFS/2 - SYNC utility'
option map=$(OBJ_DIR)\sync.map
option symfile=$(OBJ_DIR)\sync.sym
file $(SYNC_OBJS)
<<

$(READRES): $(READRES_OBJS)
	$(LINK) @<<
format os2 lx
name $(READRES)
option description '@#OS2World:0.20#@HFS/2 - READRES utility'
option map=$(OBJ_DIR)\readres.map
option symfile=$(OBJ_DIR)\readres.sym
file $(READRES_OBJS)
<<

$(BINDISK): $(BINDISK_OBJS)
	$(LINK) @<<
format os2 lx
name $(BINDISK)
option description '@#OS2World:0.20#@HFS/2 - BINDISK utility'
option map=$(OBJ_DIR)\bindisk.map
option symfile=$(OBJ_DIR)\bindisk.sym
file $(BINDISK_OBJS)
<<

$(PREPARE_EXE):
	$(MAKE) -a -z -f $(PREPARE_DIR)\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)

# Explicit rules
$(OBJ_DIR)\fskill.obj: $(SRC_DIR)\fskill.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\sync.obj: $(SRC_DIR)\sync.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\readres.obj: $(SRC_DIR)\readres.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\bindisk.obj: $(SRC_DIR)\bindisk.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(OBJ_DIR)\*.sym 2>nul
	del $(FSKILL) 2>nul
	del $(SYNC) 2>nul
	del $(READRES) 2>nul
	del $(BINDISK) 2>nul
	del $(PREPARE_EXE) 2>nul
	$(MAKE) -f $(PREPARE_DIR)\Makefile.wat clean