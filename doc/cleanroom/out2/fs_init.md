# How an IFS obtains the DevHelp entry point

GPL v2 or later. Clean-room, own text, written 2026 from public sources
(see PROVENANCE2.md; source labels A-F are defined there).

## 1. The facts

1. The kernel calls the IFS entry point `FS_INIT` once, at load time, with three
   arguments (declared `far pascal`, so pushed left to right):

   | order | name | type | meaning |
   |---|---|---|---|
   | 1 | szParm   | `char far *` (16:16) | the text after the driver name on the IFS= line of CONFIG.SYS, or NULL |
   | 2 | pDevHlp  | `unsigned long` holding a 16:16 far pointer | the kernel's DevHelp router |
   | 3 | pMiniFSD | `unsigned long far *` | hand-over area shared with the boot mini-FSD (may be NULL) |

   Return value: 0 = loaded, non-zero = load fails (error code). Evidence:
   source B (Netlabs FAT32, a 16-bit IFS: the definition of `FS_INIT` and the line that
   stores the parameter), source A (JFS, `fs_init16`, the 16-bit entry stub:
   the same three stack arguments, return 0 on success). `src\Ring0\r0comm.c`
   already has exactly this prototype.
2. The IFS has no DevHelp import. The value received in `pDevHlp` is the entry
   address; the IFS stores it in a global in its own default data segment and
   makes each DevHelp call as an indirect far call through that global, with the
   function number in `DL` and the other inputs in registers; carry flag set on
   return = error with the code in `AX`. Evidence: B (`helpers.asm`: loads the
   function number in `dl`, then `call far ptr [..]` through `_Device_Help`),
   A (`dhrouter` stores the dword, then `call dword ptr dhrouter` with `dl` =
   function), C (Open Watcom `devaux.h`: `extern ULONG Device_Help;` and
   `call dword ptr [Device_Help]` in every inline wrapper).
3. The global must be 4 bytes (a 16:16 pointer) and must be in DGROUP, because the
   wrappers read it relative to DS. Name used by the Open Watcom header:
   `Device_Help` (C symbol; with the 16-bit large model the object symbol gets the
   usual decoration, the inline wrappers handle that themselves).
4. DS: `FS_INIT` (like every FS_ entry) is declared `_loadds` in B and in
   `r0comm.c`; the kernel does not hand over DS=DGROUP.

## 2. What this means for the 16-bit build

(a) `dhcalls.lib` is not needed and must not be used. Its job was exactly the
    wrappers; they are small and need only the global `Device_Help`.

(b) What exists in public form. Source C (Open Watcom 2.0 master,
    `bld/os2api/ddk/devaux.h`) has complete inline wrappers (`#pragma aux ... =`
    machine code) for many calls, among them `DevHelp_AllocGDTSelector`,
    `DevHelp_FreeGDTSelector`, `DevHelp_SemRequest`, `DevHelp_SemClear`,
    `DevHelp_Yield` ... It only has the function-number ENUM (no wrapper) for
    the four calls HFS/2 still needs:

    | HFS/2 call site (`r0comm.c`) | function | number (hex) | registers in (from sources C and D) | on return |
    |---|---|---|---|---|
    | FS_INIT      | DevHelp_AllocGDTSelector | 2Dh (45) | ES:DI = array of selectors, CX = count | AX = rc |
    | FS_INIT      | DevHelp_VMAlloc          | 57h (87) | EAX = flags, ECX = length, EDI = physical address (-1: none) | EAX = linear address, CF = error (EAX = code) |
    | FS_FSCTL path| DevHelp_VMLock           | 55h (85) | EAX = flags, EBX = linear address, ECX = length, EDI = linear address of page list (-1: none), ESI = linear address of the lock handle | EAX = page count, CF = error |
    | FS_FSCTL path| DevHelp_LinToGDTSelector | 5Ch (92) | AX = selector, EBX = linear address, ECX = size | CF = error |
    | exit/detach  | DevHelp_VMUnLock         | 56h (86) | ESI = linear address of the lock handle | CF = error |

    Number check: C (enum), D (assembler of the xwpsec driver, uses the same
    names), B (2Dh, 53h, 5Ch). The register assignment of VMAlloc/VMLock/VMUnLock
    comes from D (`sec32_devhlp32.asm`, 32-bit thunks for the same 16-bit calls;
    they load exactly these registers) and E (uniaud16's local devhelp.h, whose
    pragmas load the same registers). The output register of VMAlloc (EAX) and
    of VMLock (EAX = page count) is read from D. UNVERIFIED: the 16:16 "SelOffset"
    output that the old `dhcalls.h` prototype of `DevHelp_VMAlloc` has: HFS/2
    never reads it, so the new wrapper simply has no such parameter.

(c) Which header supplies `Device_Help`. Take ONE definition: the Open Watcom
    header declares it as `unsigned long`; HFS/2's `r0global.h/.c` declare it
    as `PFN`. Do not include both with the two types. Either use `ULONG` in
    `r0global.*` (and cast at use) or do not include `devaux.h` and write the
    five wrappers below yourself. The second way avoids any dependency on whether
    the installed Open Watcom `h\os2` has `devaux.h` (UNVERIFIED: whether the
    release installs it there; it is in the source tree under `bld/os2api/ddk`).

(d) The wrappers and DS. They read `Device_Help` through DS. Compile the Ring 0
    code with `-zu` (SS is the kernel stack, not DGROUP: Open Watcom
    documents `-zu` for DLL-type code that runs on the caller's stack) and
    `-s` (no stack-check calls; the check routine would use runtime variables that
    nobody initialised). Source F. To make the wrapper itself independent of
    the caller's DS, mark it with the pragma modifier that loads DS first
    (`parm loadds`, documented in the Open Watcom pragma chapter, F). UNVERIFIED:
    that `parm loadds` is honoured on inline-code (`=`) pragmas, not
    just on ordinary functions. If it is not, put the five wrappers in a
    hand-written `.asm` module that begins with `mov ax, DGROUP / mov ds, ax`.

## 3. Original C snippet

Store the pointer in FS_INIT first (before anything uses DevHelp):

```c
/* r0global.h */
extern unsigned long Device_Help;   /* 16:16 far pointer to the kernel's DevHelp router */

/* r0global.c */
unsigned long Device_Help = 0UL;    /* initialised data: lives in DGROUP image */

/* r0comm.c, first statements of FS_INIT */
short int far pascal _export _loadds FS_INIT(char far *szParm,
                                             unsigned long pDevHlp,
                                             unsigned long far *pMiniFSD)
{
    Device_Help = pDevHlp;          /* must come before any DevHelp_* call */
    ...
}
```

Wrappers for the calls that the Open Watcom header does not wrap (sketch,
original text, UNTESTED: not compiled, since no Open Watcom is available here). Use
`wcc -ml -zu -s -3`. Numbers and registers are from the table in 2(b).

```c
#define DH_CALL        "call dword ptr [Device_Help]"   /* DL already set */
/* CF set => AX holds the error code; else clear AX */
#define DH_RC          "jc  short hf1" "xor ax,ax" "hf1:"

unsigned short DH_VMUnLock(unsigned long lockHandleLin);
#pragma aux DH_VMUnLock = \
    "mov dl,56h" DH_CALL DH_RC \
    parm loadds [esi] value [ax] modify exact [ax dl];

unsigned short DH_LinToGDTSelector(unsigned short sel, unsigned long lin,
                                   unsigned long size);
#pragma aux DH_LinToGDTSelector = \
    "mov dl,5Ch" DH_CALL DH_RC \
    parm loadds [ax] [ebx] [ecx] value [ax] modify exact [ax dl];
```

For VMAlloc and VMLock the result (EAX) must be returned together with the carry
flag, which a pragma cannot express cleanly; write them in a small `.asm` file
(wasm) that is called `far pascal`, loads DS = DGROUP, sets the registers of the
table, does the indirect call through `Device_Help`, and stores EAX through a
pointer argument only when CF is clear. That also removes the 16:16 versus
linear-address question: both lock-handle and page-list arguments are LINEAR
addresses (32-bit) in registers, as `r0comm.c` already passes them (`Lock1` is
the linear address returned by VMAlloc; the page list is -1).

## 4. Consequences for the other two build problems

- `src\Ring0\fshelper.c` must leave the link (its DevHelp emulation was wrong
  for a real IFS); the real helpers come from `fshelper_imports.lnk`.
  The unused `MFSH_*` stubs go with it.
- `FSH_REGISTERPERFCTRS` was emulated with `DevHelp_RegisterPerfCtrs`, which is not
  even present in the header copies checked; it is now a plain FSHELPER import.

## 5. Not established

- Whether the kernel passes anything useful in `pMiniFSD` for an installed
  (not boot-time) IFS: HFS/2 and FAT32 ignore it. Leave unused.
- Exact stack depth/limits on the kernel stack for the DevHelp calls: not
  documented in the allowed sources.
