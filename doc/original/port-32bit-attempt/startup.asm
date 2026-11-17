; startup.asm - Minimal startup for HFS/2 IFS driver (Open Watcom)
; Replaces Borland's c0.asm with Watcom-compatible version
;
; This provides the Startup symbol expected by the linker
; For an IFS driver, we don't need full C runtime initialization
; The IFS entry point is FS_INIT, not main()

                .386

_TEXT           segment dword public 'CODE'
                assume  cs:_TEXT

                public  Startup
Startup         proc    far
                ; For IFS driver, we don't run standard C initialization
                ; The IFS entry points (FS_INIT, etc.) are called directly by the kernel
                ; Just return - the IFS will be initialized via FS_INIT
                ret
Startup         endp

; Also provide _cstart_ alias for Watcom compatibility
                public  _cstart_
_cstart_        proc    far
                ret
_cstart_        endp

_TEXT           ends

                end