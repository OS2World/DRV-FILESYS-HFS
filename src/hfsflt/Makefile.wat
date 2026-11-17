# Open Watcom 2.0 Makefile for HFS/2 Filter Driver

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\hfsflt
DDK_BASE = C:\MiniDDK

# Output
NAME = hfsflt
BIN_DIR = $(PROJ_ROOT)\bin
OBJ_DIR = $(BIN_DIR)\obj\hfsflt
FLT = $(BIN_DIR)\$(NAME).flt
DEF = $(SRC_DIR)\$(NAME).def

# Compiler flags for 16-bit (wcc)
# -ms: small memory model
# -bt=os2: target OS/2
# -zc: preserve case / const in code segment
# -ot: optimization for time
# -d: define macros
# Note: __WATCOMC__ is predefined by the compiler, don't redefine it
# MiniDDK at C:\MiniDDK for DDK headers
# 16-bit headers in $(WATCOM)\h and $(WATCOM)\h\os2
CFLAGS = -ms -bt=os2 -zc -ot -omi -s -d__OS2__ -I$(WATCOM)\h -I$(WATCOM)\h\os2 -I$(PROJ_ROOT)\src\include -I$(DDK_BASE)\base\h

# Assembly flags
# -ms: small memory model
# -i: include path (use local C:\temp for devhdr.inc)
# Note: __WATCOMC__ is predefined, -mx not supported by wasm
# Note: -d__LARGE__ -d__OS2__ cause "Label already defined" error
AFLAGS = -ms -i$(SRC_DIR)

# Linker flags - 16-bit filter driver
LFLAGS = name $(FLT) format os2 dll initglobal option protmode option stack=4096 option nocaseexact option map=$(OBJ_DIR)\$(NAME).map option symfile=$(OBJ_DIR)\$(NAME).sym library $(WATCOM)\lib286\os2\clibs.lib library $(WATCOM)\lib286\math87s.lib library $(WATCOM)\lib286\os2\emu87.lib library $(WATCOM)\lib286\os2\os2.lib library $(DDK_BASE)\base\lib\dhcalls.lib library $(DDK_BASE)\base\lib\addcalls.lib

# Objects
COBJS = $(OBJ_DIR)\boot.obj $(OBJ_DIR)\filter.obj $(OBJ_DIR)\globals.obj $(OBJ_DIR)\init.obj $(OBJ_DIR)\initutil.obj $(OBJ_DIR)\io.obj $(OBJ_DIR)\log.obj $(OBJ_DIR)\strat1.obj $(OBJ_DIR)\vsprintf.obj
AOBJS = $(OBJ_DIR)\segs.obj
OBJS = $(COBJS) $(AOBJS)

all: $(FLT)

$(FLT): $(OBJS) $(DEF)
	$(LINK) @<<
$(LFLAGS)
option description '@#OS2World:0.20#@HFS/2 Diskette Filter Driver'
file $(OBJ_DIR)\boot.obj
file $(OBJ_DIR)\filter.obj
file $(OBJ_DIR)\globals.obj
file $(OBJ_DIR)\init.obj
file $(OBJ_DIR)\initutil.obj
file $(OBJ_DIR)\io.obj
file $(OBJ_DIR)\log.obj
file $(OBJ_DIR)\strat1.obj
file $(OBJ_DIR)\vsprintf.obj
file $(OBJ_DIR)\segs.obj
segment type DATA SHARED PRELOAD
segment '_TEXT' PRELOAD IOPL
segment Code PRELOAD IOPL
segment LIBCODE PRELOAD IOPL
<<

# Explicit rules
$(OBJ_DIR)\boot.obj: $(SRC_DIR)\boot.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\filter.obj: $(SRC_DIR)\filter.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\globals.obj: $(SRC_DIR)\globals.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\init.obj: $(SRC_DIR)\init.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\initutil.obj: $(SRC_DIR)\initutil.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\io.obj: $(SRC_DIR)\io.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\log.obj: $(SRC_DIR)\log.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\strat1.obj: $(SRC_DIR)\strat1.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\vsprintf.obj: $(SRC_DIR)\vsprintf.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) -fo=$@ $<

$(OBJ_DIR)\segs.obj: $(SRC_DIR)\segs.asm
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(ASM) $(AFLAGS) $< -fo=$@

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(OBJ_DIR)\*.sym 2>nul
	del $(FLT) 2>nul