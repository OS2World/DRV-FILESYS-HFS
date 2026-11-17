# Open Watcom 2.0 Makefile for the 16-bit HFS/2 UHFS DLL

WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc.exe
LINK = $(WATCOM)\binp\wlink.exe
MAKE = $(WATCOM)\binp\wmake.exe

PROJ_ROOT = .
SRC_DIR = $(PROJ_ROOT)\src\UHFS
LIBHFS_DIR = $(SRC_DIR)\libhfs

# Output
NAME = uhfs
BIN_DIR = $(PROJ_ROOT)\bin
OBJ_DIR = $(BIN_DIR)\obj\uhfs
DLL = $(PROJ_ROOT)\dll\$(NAME).dll
LIBFILE = $(PROJ_ROOT)\lib\libhfs16.lib
DEF = $(NAME).def

# UHFS uses the original 16-bit OS/2 APIs and NE module format.
CFLAGS = -bt=os2 -ml -zc -3 -ot -d__OS2__ -dNODEVLOCKS -dUHFS -I$(WATCOM)\h\os21x -I$(WATCOM)\h -I$(PROJ_ROOT)\src\include -I$(LIBHFS_DIR)

# Linker flags - DLL
LFLAGS = name $(DLL) format os2 dll option map=$(OBJ_DIR)\$(NAME).map option symfile=$(OBJ_DIR)\$(NAME).sym library $(LIBFILE) library $(WATCOM)\lib286\os2\clibdll.lib library C:\MiniDDK\base\lib\os2286.lib

# Objects
COBJS = $(OBJ_DIR)\chkdsk.obj $(OBJ_DIR)\format.obj $(OBJ_DIR)\misc.obj $(OBJ_DIR)\recover.obj $(OBJ_DIR)\sys.obj
OBJS = $(COBJS)

all: $(LIBFILE) $(DLL)

$(LIBFILE):
	$(MAKE) -a -z -f $(LIBHFS_DIR)\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)

$(DLL): $(OBJS) $(LIBFILE) $(SRC_DIR)\$(DEF)
	$(LINK) @<<
$(LFLAGS)
option description '@#OS2World:0.20#@HFS/2 - A Hierarchical File System Driver for OS/2'
export CHKDSK
export FORMAT
export RECOVER
export SYS
file $(OBJ_DIR)\chkdsk.obj
file $(OBJ_DIR)\format.obj
file $(OBJ_DIR)\misc.obj
file $(OBJ_DIR)\recover.obj
file $(OBJ_DIR)\sys.obj
<<

# Explicit rules
$(OBJ_DIR)\chkdsk.obj: $(SRC_DIR)\chkdsk.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\format.obj: $(SRC_DIR)\format.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\misc.obj: $(SRC_DIR)\misc.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\recover.obj: $(SRC_DIR)\recover.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

$(OBJ_DIR)\sys.obj: $(SRC_DIR)\sys.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -fo=$@

clean:
	del $(OBJ_DIR)\*.obj 2>nul
	del $(OBJ_DIR)\*.map 2>nul
	del $(OBJ_DIR)\*.sym 2>nul
	del $(DLL) 2>nul
	del $(LIBFILE) 2>nul
	$(MAKE) -f $(LIBHFS_DIR)\Makefile.wat clean