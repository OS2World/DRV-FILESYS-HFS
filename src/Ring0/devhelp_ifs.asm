; devhelp_ifs.asm - DevHelp calls for an IFS (16-bit, large model, Open Watcom wasm)
;
; HFS/2 - A Hierarchical File System Driver for OS/2
; GPL v2 or later.  Written 2026 for the Open Watcom port; function numbers and
; register use from doc\cleanroom\out2\fs_init.md.
;
; An IFS receives the DevHelp entry point in FS_INIT and stores it in the
; global Device_Help (r0global.c).  Each call: function number in DL, indirect
; far call through Device_Help, carry set = error code in AX.
; All routines are "far pascal": arguments pushed left to right, callee pops.
; DS must be DGROUP (all FS_ entry points are _loadds).

                .386

DGROUP          group   _DATA
_DATA           segment word public use16 'DATA'
                extrn   _Device_Help:dword
_DATA           ends

_TEXT           segment word public use16 'CODE'
                assume  cs:_TEXT, ds:DGROUP

; marker symbol every 16-bit Watcom object references; no run-time start code is used
                public  _small_code_
_small_code_    label   byte

; unsigned short far pascal HFS_VMAlloc(unsigned long flags, unsigned long size,
;                  unsigned long phys, unsigned long far *lin, void far *unused)
                public  HFS_VMALLOC
HFS_VMALLOC     proc    far
                push    bp
                mov     bp, sp
                push    esi
                push    edi
                mov     eax, [bp+22]            ; flags
                mov     ecx, [bp+18]            ; size
                mov     edi, [bp+14]            ; physical address (-1: none)
                mov     dl, 57h
                call    dword ptr _Device_Help
                jc      short vma_err
                les     bx, [bp+10]             ; lin
                mov     es:[bx], eax
                xor     ax, ax
vma_err:        pop     edi
                pop     esi
                pop     bp
                ret     20
HFS_VMALLOC     endp

; unsigned short far pascal HFS_VMLock(unsigned long flags, unsigned long lin,
;                  unsigned long size, unsigned long pagelist,
;                  unsigned long lockhandle, unsigned long far *pagecount)
                public  HFS_VMLOCK
HFS_VMLOCK      proc    far
                push    bp
                mov     bp, sp
                push    esi
                push    edi
                push    ebx
                mov     eax, [bp+26]            ; flags
                mov     ebx, [bp+22]            ; linear address
                mov     ecx, [bp+18]            ; length
                mov     edi, [bp+14]            ; page list (-1: none)
                mov     esi, [bp+10]            ; lock handle (linear)
                mov     dl, 55h
                call    dword ptr _Device_Help
                jc      short vml_err
                les     bx, [bp+6]              ; page count
                mov     es:[bx], eax
                xor     ax, ax
vml_err:        pop     ebx
                pop     edi
                pop     esi
                pop     bp
                ret     24
HFS_VMLOCK      endp

; unsigned short far pascal HFS_VMUnLock(unsigned long lockhandle)
                public  HFS_VMUNLOCK
HFS_VMUNLOCK    proc    far
                push    bp
                mov     bp, sp
                push    esi
                mov     esi, [bp+6]
                mov     dl, 56h
                call    dword ptr _Device_Help
                jc      short vmu_err
                xor     ax, ax
vmu_err:        pop     esi
                pop     bp
                ret     4
HFS_VMUNLOCK    endp

; unsigned short far pascal HFS_LinToGDTSelector(unsigned short sel,
;                  unsigned long lin, unsigned long size)
                public  HFS_LINTOGDTSELECTOR
HFS_LINTOGDTSELECTOR proc far
                push    bp
                mov     bp, sp
                push    ebx
                mov     ax, [bp+14]             ; selector
                mov     ebx, [bp+10]            ; linear address
                mov     ecx, [bp+6]             ; size
                mov     dl, 5Ch
                call    dword ptr _Device_Help
                jc      short l2g_err
                xor     ax, ax
l2g_err:        pop     ebx
                pop     bp
                ret     10
HFS_LINTOGDTSELECTOR endp

; unsigned short far pascal HFS_AllocGDTSelector(unsigned short far *sels,
;                  unsigned short count)
                public  HFS_ALLOCGDTSELECTOR
HFS_ALLOCGDTSELECTOR proc far
                push    bp
                mov     bp, sp
                push    di
                mov     cx, [bp+6]              ; count
                les     di, [bp+8]              ; selector array
                mov     dl, 2Dh
                call    dword ptr _Device_Help
                jc      short agd_err
                xor     ax, ax
agd_err:        pop     di
                pop     bp
                ret     6
HFS_ALLOCGDTSELECTOR endp

_TEXT           ends
                end
