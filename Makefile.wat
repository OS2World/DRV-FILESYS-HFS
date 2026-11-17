# Open Watcom 2.0 Makefile for HFS/2
# Run from project root

# Watcom environment setup
WATCOM = C:\WATCOM
CC = $(WATCOM)\binp\wcc386.exe
ASM = $(WATCOM)\binp\wasm.exe
LINK = $(WATCOM)\binp\wlink.exe
RC = $(WATCOM)\binp\wrc.exe
MAKE = $(WATCOM)\binp\wmake.exe
PROJ_ROOT = .

CFLAGS = -ms -zc -DOS2 -I$(WATCOM)\h -I$(WATCOM)\h\os2 -Isrc\include

all:
	@echo Building HFS/2 ...
	$(MAKE) -a -z -f $(PROJ_ROOT)\src\Ring0\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)
	$(MAKE) -a -z -f $(PROJ_ROOT)\src\Ring3\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)
	$(MAKE) -a -z -f $(PROJ_ROOT)\src\hfsflt\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)
	$(MAKE) -a -z -f $(PROJ_ROOT)\src\UHFS\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)
	$(MAKE) -a -z -f $(PROJ_ROOT)\src\util\Makefile.wat PROJ_ROOT=$(PROJ_ROOT)

clean:
	@echo Cleaning ...
	$(MAKE) -f $(PROJ_ROOT)\src\Ring0\Makefile.wat PROJ_ROOT=$(PROJ_ROOT) clean
	$(MAKE) -f $(PROJ_ROOT)\src\Ring3\Makefile.wat PROJ_ROOT=$(PROJ_ROOT) clean
	$(MAKE) -f $(PROJ_ROOT)\src\hfsflt\Makefile.wat PROJ_ROOT=$(PROJ_ROOT) clean
	$(MAKE) -f $(PROJ_ROOT)\src\UHFS\Makefile.wat PROJ_ROOT=$(PROJ_ROOT) clean
	$(MAKE) -f $(PROJ_ROOT)\src\util\Makefile.wat PROJ_ROOT=$(PROJ_ROOT) clean