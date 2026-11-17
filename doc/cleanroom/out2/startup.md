# What an IFS needs instead of the old `Startup` symbol

GPL v2 or later. Clean-room, own text, written 2026 (see PROVENANCE2.md).

## Answer in one line

Nothing is needed from the kernel side. An IFS has no `main` and no C runtime
start-up that the kernel calls; the only entry the kernel calls first is
`FS_INIT`. The old `Startup()` call in `r0comm.c` (line 84) can be deleted.
Recommended replacement: nothing at all, or the empty macro
`#define Startup() ((void)0)` if you want the source to stay close to the original.

## Evidence

- Two real, shipping IFS projects have no start-up routine and no entry symbol:
  Netlabs FAT32 (16-bit; LGPL-2.1) and JFS for OS/2 (GPL-2). Their `.def` files
  contain `LIBRARY`, `PROTMODE`, `DATA PRELOAD SINGLE ...`, `CODE PRELOAD ...`
  and the `FS_*` exports; no `INITGLOBAL`/`INITINSTANCE`, no `STUB`, no entry
  stub. `FS_INIT` begins with its own work: store DevHelp, parse the line
  (B: `FS_INIT` in `fat32a.c`; A: `fs_init16` in `thunk.asm`; the `.def`
  text in the JFS makefile). JFS's `FS_INIT` does not run a C start-up either:
  its 16-bit stub reads the arguments, sets the flat environment and calls
  into the C code.
- Open Watcom linker guide (F): for OS/2 `DLL` output the "initialization
  routine is defined by the start address"; an initialization routine is run
  only if a start address exists. Do not give the link a start address
  (`option start=...` absent, no `_cstart_`/`__DLLstart_` pulled in) and no
  routine is run on load.

## Consequences for `hfs.ifs`

1. Delete `src\Ring0\startup.asm`/`startup.c` and remove them from the object
   list. The old `Startup` and the `_cstart_` alias in those files returned with
   an undefined AX; they could only do harm if the linker made one of them the
   entry point of an INITGLOBAL DLL (the default if a start address exists).
2. Link with `option nodefaultlibs` (documented, F) so the compiler's default
   library records do not drag in the Open Watcom run-time start code or
   a runtime library that expects initialisation, and add explicitly only what
   the code really uses (`library clibl.lib` for string functions such as
   `memcpy`, `strlen`, `memset`, if they are used). UNVERIFIED: which clib
   members those functions need and whether they touch uninitialised runtime
   variables; check the map file after the first link: no `__STK`, no
   `__DLLstart_`, no `__InitRtns`, no `stdio`/heap members.
3. Compile options: `-ml -3 -zu -s` (and not `-bd`: this is not an ordinary DLL).
   `-zu` (SS != DGROUP), `-s` (no stack-overflow checks) are documented in F.
   Reason: kernel stack at entry, no runtime to feed the stack checker.
4. Data initialisation. Without C start-up nobody zero-fills uninitialised
   statics and nobody runs C constructors. UNVERIFIED: whether the kernel's loader
   zero-fills the DGROUP BSS of an IFS (FAT32 does `memset` on its own
   parameter block in `FS_INIT`, which suggests the author did not rely on it). To be safe,
   initialise every global that matters explicitly (`= 0` gives an initialised
   image; or clear in `FS_INIT`). The old Borland `Startup()` probably zeroed
   `_BSS`; the new code should do that explicitly instead of reintroducing a routine.
   (The Borland `c0.asm` was not studied beyond its banner; see PROVENANCE2.md.)
5. Exports stay as in `hfs.def`; do not add `INITGLOBAL`/`INITINSTANCE`.

## Minimal original replacement (only if a C-visible `Startup` is wanted)

```c
/* r0comm.h: nothing is required of the kernel; kept only so FS_INIT reads
   like before.  Remove the call and this macro together. */
#define Startup() ((void)0)
```

## Not established

- Whether `wlink` with `system os2 dll` and no start address emits a DLL with
  CS:IP = 0 (no entry) or complains. Open Watcom was not available here.
  If the linker demands a start symbol, give it a routine that does nothing
  and returns 1 in AX (the ring 3 DLL convention for "initialisation OK"),
  and add `INITGLOBAL` knowledge to this file after testing on the Dev VM
  whether the kernel even calls it. UNVERIFIED.
