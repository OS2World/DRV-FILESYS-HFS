; test
include devhdr.inc

DDHeader segment dword public 'DATA'

DiskDDHeader dd -1
dw DEVLEV_3 + DEV_CHAR_DEV
dw _Strat1
dw 0
db 'HFSFLT$ '
dw 0
dw 0
dw 0
dw 0
dd DEV_ADAPTER_DD
dw 0

DDHeader ends

LIBDATA segment dword public 'DATA'
LIBDATA ends

_TEXT segment dword public 'CODE'
extrn _Strat1:near
_TEXT ends

_DATA segment dword public 'DATA'
_DATA ends

_BSS segment dword public 'BSS'
_BSS ends

Code segment dword public 'CODE'
Code ends

LIBCODE segment dword public 'CODE'
LIBCODE ends

DGROUP group _BSS, DDHeader, LIBDATA, _DATA
StaticGroup group Code, LIBCODE, _TEXT
end