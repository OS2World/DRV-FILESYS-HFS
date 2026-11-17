# Open Watcom 2.0 Makefile for HFS/2 Ring 0 IFS Driver

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc386.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe
LIB = $(WATCOM)\binp\wlib.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\Ring0
DDK_BASE = C:\MiniDDK

# Output
NAME = hfs
BIN_DIR = $(PROJ_ROOT)\bin
IFS = $(BIN_DIR)\$(NAME).ifs
DEF = $(SRC_DIR)\$(NAME).def

OBJ_DIR = $(BIN_DIR)\obj\ring0

# Compiler flags
# -ml: large model (16-bit)
# -zc: preserve case
# -3: 386 instructions
# -ot: optimization for time
# -zK: disable keyword extensions (far, near, pascal, etc.)
# -d: define macros
# -i: include path
CFLAGS = -ml -zc -3 -ot -zK -d__OS2__ -dIFS -I$(WATCOM)\h -I$(WATCOM)\h\os2 -I$(DDK_BASE)\base\h -I$(DDK_BASE)\base\inc -I$(PROJ_ROOT) -I$(PROJ_ROOT)\src\include -I$(SRC_DIR)

# Assembly flags
AFLAGS = -ml -3p -zcm=masm -d__LARGE__ -d__OS2__ -d__WATCOMC__

# Linker flags
# format os2 lx for 32-bit IFS
# Using Watcom's os2386.lib and clib3s.lib from C:\WATCOM\lib386\os2\
LFLAGS = format os2 lx file $(IFS) name $(NAME) option map=$(OBJ_DIR)\$(NAME).map option symfile=$(OBJ_DIR)\$(NAME).sym library $(WATCOM)\lib386\os2\clib3s.lib library $(WATCOM)\lib386\os2\os2386.lib library $(DDK_BASE)\base\lib\dhcalls.lib

# Objects
OBJS = $(OBJ_DIR)\startup.obj $(OBJ_DIR)\stubs.obj $(OBJ_DIR)\r0comm.obj $(OBJ_DIR)\r0global.obj $(OBJ_DIR)\log.obj $(OBJ_DIR)\vsprintf.obj $(OBJ_DIR)\fshelper.obj

all: $(IFS)

$(IFS): $(OBJS) $(DEF)
	$(LINK) @<<
format os2 lx
name $(IFS)
option description '@#OS2World:0.20#@HFS/2 - A Hierarchical File System Driver for OS/2'
option map=$(OBJ_DIR)\$(NAME).map
option symfile=$(OBJ_DIR)\$(NAME).sym
library $(WATCOM)\lib386\os2\clib3r.lib
library $(WATCOM)\lib386\os2\os2386.lib
library $(WATCOM)\lib386\math387r.lib
library $(WATCOM)\lib386\noemu387.lib
library $(DDK_BASE)\base\lib\dhcalls.lib
file $(OBJ_DIR)\startup.obj
file $(OBJ_DIR)\stubs.obj
file $(OBJ_DIR)\r0comm.obj
file $(OBJ_DIR)\r0global.obj
file $(OBJ_DIR)\log.obj
file $(OBJ_DIR)\vsprintf.obj
file $(OBJ_DIR)\fshelper.obj
<<

# Explicit rules for each object file
$(OBJ_DIR)\startup.obj: $(SRC_DIR)\startup.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

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

$(OBJ_DIR)\fshelper.obj: $(SRC_DIR)\fshelper.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(OBJ_DIR)\*.sym 2>nul
	del $(IFS) 2>nul