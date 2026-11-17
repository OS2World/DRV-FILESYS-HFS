# Open Watcom 2.0 Makefile for the HFS/2 Ring 0 IFS (hfs.ifs)
#
# The kernel calls the FS_* entry points as 16-bit far pascal routines, so the
# IFS must be a 16-bit NE library module: compile with the 16-bit compiler
# (wcc, large model) and link with "system os2 dll".  The kernel helper calls
# (FSH_*) are imported by ordinal from the module FSHELPER
# (doc\cleanroom\out2\fshelper_imports.lnk); DevHelp calls go through the
# entry point FS_INIT receives (devhelp_ifs.asm).  No C run-time start-up.

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\Ring0
DDK_BASE = C:\MiniDDK

NAME = hfs
BIN_DIR = $(PROJ_ROOT)\bin
IFS = $(BIN_DIR)\$(NAME).ifs
OBJ_DIR = $(BIN_DIR)\obj\ring0
IMPORTS = $(PROJ_ROOT)\doc\cleanroom\out2\fshelper_imports.lnk
EXPORTS = $(SRC_DIR)\hfs_exports.lnk

# -ml        large model
# -zu        SS != DS (the kernel stack is used)
# -zdp -zff -zgf   DS pegged to DGROUP, FS/GS floating
# -zl        no default library records;  -s  no stack checks (no run-time)
# include path: src\include16 holds a shadow os2.h (adds PPVOID for the DDK headers)
CFLAGS = -ml -bt=os2 -zdp -zff -zgf -zu -zl -s -zq -ot -3 -d__OS2__ -dIFS -i=$(PROJ_ROOT)\src\include16 -i=$(WATCOM)\h -i=$(WATCOM)\h\os21x -i=$(DDK_BASE)\base\h -i=$(DDK_BASE)\base\inc -i=$(PROJ_ROOT) -i=$(PROJ_ROOT)\src\include -i=$(SRC_DIR)

OBJS = $(OBJ_DIR)\stubs.obj $(OBJ_DIR)\r0comm.obj $(OBJ_DIR)\r0global.obj $(OBJ_DIR)\log.obj $(OBJ_DIR)\vsprintf.obj $(OBJ_DIR)\hfs_rt.obj $(OBJ_DIR)\devhelp_ifs.obj

all: $(IFS)

$(IFS): $(OBJS) $(EXPORTS) $(IMPORTS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(LINK) @<<
name $(IFS)
system os2 dll
option protmode
option stack=0
option nodefaultlibs
option quiet
option map=$(OBJ_DIR)\$(NAME).map
option description '@#OS2World:0.20#@HFS/2 - A Hierarchical File System Driver for OS/2'
library $(WATCOM)\lib286\os2\os2.lib
library $(WATCOM)\lib286\os2\clibl.lib
file $(OBJ_DIR)\stubs.obj
file $(OBJ_DIR)\r0comm.obj
file $(OBJ_DIR)\r0global.obj
file $(OBJ_DIR)\log.obj
file $(OBJ_DIR)\vsprintf.obj
file $(OBJ_DIR)\hfs_rt.obj
file $(OBJ_DIR)\devhelp_ifs.obj
@$(IMPORTS)
segment type CODE PRELOAD EXECUTEREAD NONSHARED
segment type DATA PRELOAD NONSHARED
@$(EXPORTS)
<<

$(OBJ_DIR)\stubs.obj: $(SRC_DIR)\stubs.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\r0comm.obj: $(SRC_DIR)\r0comm.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\r0global.obj: $(SRC_DIR)\r0global.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\log.obj: $(SRC_DIR)\log.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\vsprintf.obj: $(SRC_DIR)\vsprintf.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\hfs_rt.obj: $(SRC_DIR)\hfs_rt.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\devhelp_ifs.obj: $(SRC_DIR)\devhelp_ifs.asm
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(ASM) -ml -3 -zq -fo=$@ $<

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(IFS) 2>nul
