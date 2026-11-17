;
; HFS/2 - A Hierarchical File System Driver for OS/2
; Copyright (C) 1996, 1997 Marcus Better
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;
; Clean-room replacement, written 2026 from public documentation; see PROVENANCE.md
;
; devhdr.inc
; Constants for the device driver header that segs.asm lays out.
; The header holds, in order: next-header link, attribute word, strategy
; offset, IDC offset, 8-byte name, four selector words, capability dword.

; ---- bits of the attribute word ------------------------------------
DEV_CHAR_DEV    equ     8000h   ; character-device bit
DEV_IDC         equ     4000h   ; driver has an inter-driver entry
DEV_OPENCLOSE   equ     0800h   ; wants open/close requests
DEV_GIOCTL      equ     0040h   ; generic IOCtl supported

; function level field (bits 7-9 of the attribute word)
DEVLEV_0        equ     0000h
DEVLEV_1        equ     0080h
DEVLEV_2        equ     0100h
DEVLEV_3        equ     0180h   ; level 3: capability dword is present
DEVLEV_MASK     equ     0380h

; ---- bits of the capability dword (last field of the header) --------
; UNVERIFIED: the public header lists a bit named "participates in the
; ADD strategy" (08h); that it is the one segs.asm means by DEV_ADAPTER_DD
; is an identification by meaning, not by name.
DEV_ADAPTER_DD  equ     00000008h
