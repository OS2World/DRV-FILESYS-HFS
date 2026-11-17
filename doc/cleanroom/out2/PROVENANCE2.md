# Provenance, second clean-room pass (FSHELPER imports, FS_INIT/DevHelp, start-up)

Written 2026-11-17 against `doc/cleanroom/SPEC.md` section 2. Only files in
`doc/cleanroom/out2/` were written; `src\` was not modified; nothing built, nothing
committed. Open Watcom is not installed on this PC, so nothing here has been through
a compiler or linker.

## Sources

| id | source | licence | what was taken |
|---|---|---|---|
| A | osfree-project/openjfs (JFS for OS/2), https://github.com/osfree-project/openjfs : `lib/fshelper.def`, `lib/makefile`, `src-other/jfs/ifs/thunk.asm` (routine `fs_init16`), `devhelp.asm`, `makefile` (`jfs.def` text) | repository COPYING = GPL v2; file headers: GPL v2 or later (IBM copyright + GPL grant). `fshelper.def` has no header, covered by the repository licence | module name FSHELPER and ordinals 1-55 with the names; FS_INIT three arguments in stack order; DevHelp pointer stored as a dword and called with `dl` = function; no entry symbol in the `.def` |
| B | osfree-project/fat32 (Netlabs FAT32, cloned from svn.netlabs.org), https://github.com/osfree-project/fat32 : `src/ifs/fat32a.c` (`FS_INIT`, `Device_Help`), `src/ifs/helpers.asm`, `src/lib/fat32.def`, `src/lib/makefile` | README: LGPL v2.1 (compatible with GPL v2+) | independent 16-bit confirmation of the `FS_INIT(char far *, unsigned long pDevHlp, unsigned long far *)` signature, `Device_Help` as a 4-byte global in DGROUP, `call far ptr [Device_Help]` with function number in `dl`, numbers 2Dh/53h/5Ch; `.def` without start-up; link line has no start object of its own. Used as facts only |
| C | Open Watcom v2, https://github.com/open-watcom/open-watcom-v2 , `bld/os2api/ddk/devaux.h` (master) | Open Watcom Public License | `extern ULONG Device_Help;` + `call dword ptr [Device_Help]` convention; function number enum (AllocGDTSelector 45, VMLock 85, VMUnlock 86, VMAlloc 87, LinToGDTSelector 92); which DevHelp calls have wrappers (VMAlloc/VMLock/VMUnLock/LinToGDTSelector have none) |
| D | osfree-project/osfree, `OS2/WPS/contrib/src/xwpsec_ring0/sec32_devhlp32.asm` (XWorkplace security driver; derived from MWDD32 by M. Willm) | GPL v2 (file header: XWorkplace, GPL v2) | register assignment and outputs of VMAlloc, VMLock, VMUnlock, LinToGDTSelector, AllocGDTSelector. Facts only |
| E | local copy of a DD header set shipped with the uniaud16 project: `C:\Temporal\1.- OS2\Projects\Drivers\uniaud16\owddk\devhelp.h` | header says "Open Watcom Project, (c) The Open Watcom Contributors"; the project's own `readme.txt` says this file was gathered "from source found via internet search engines" and it is NOT present in the Open Watcom tree (checked) | second look at the function numbers and registers (agree with C and D). Because its origin is uncertain it was used only as a cross-check, no wrapper text taken |
| F | Open Watcom v2 documentation sources, `docs/doc/lg/ldimport.gml`, `wlbmain.gml`, `wlbimp.gml`, `ldformat.gml`, `opnodefa.gml`, `opstart.gml`, `bld/wl/lnk/specs.sp`; `docs/doc/cmn/pragma.gml` (predefined `__pascal` alias, `parm loadds`); `docs/doc/cg/cpopts.gml` (`-zu`, `-s`) | Open Watcom Public License | wlink IMPORT syntax, wlib `++` syntax, `os2_dll` system, the upper-case naming of `__pascal`, options `-zu`, `-s`, `option nodefaultlibs` |
| G | VirtualBox OSE, https://github.com/VirtualBox/virtualbox , `src/VBox/Runtime/r0drv/os2/os2imports.imp` | VirtualBox OSE: GPL-3.0-only (also offered under CDDL) | independent second table of FSHELPER ordinals 1-55 (identical to A) |
| H | HFS/2's own GPL files read: `src/Ring0/r0comm.c`, `r0global.c/.h`, `fshelper.c`, `stubs.c`, `hfs.def`, `startup.c`, `startup.asm`, `Makefile.wat` | GPL v2+ | how the code calls DevHelp, where `Device_Help` is declared, what the old start-up was, which symbols it needs |

Sourcegraph's public search API was used only to locate files (A-G); the raw files were then
fetched with curl. Downloaded copies are in the scratch folder only.

## Per deliverable

- `fshelper_imports.lnk`: module name and every ordinal from A and G (two independent
  tables, identical); IMPORT syntax from F; internal-name spelling (upper case) from F
  (pascal alias). The used-name list is the one in the task (37 names); the remaining entries
  of the module are commented.
- `fs_init.md`: FS_INIT arguments from B and A, DevHelp convention from B, A, C, register
  tables from C, D, E, build options from F. Snippets are original.
- `startup.md`: evidence from the `.def`/link practice of A and B, linker guide F.

## UNVERIFIED list

1. Module name `FSHELPER` and ordinals for the 16-bit NE format. Both tables (A, G) are
   consumed by 32-bit LX code (JFS, VBoxSF); they are two independent sources for
   module name and ordinal, but no allowed source shows the import record of a 16-bit
   NE IFS. All 37 used names are present with the same ordinal in both sources, so by the task rule
   none is marked UNVERIFIED individually; the 16-bit-NE caveat applies to all. Settle by looking at the link
   result or the loading on the Dev VM. (An installed 16-bit `.IFS` could have been parsed for its import
   table, but this was not done: see "Exposure".)
2. wlib spelling for "import by ordinal only" (`++sym.dll..export.ordinal`): documented
   grammar, not run. The `IMPORT` lines are the primary route.
3. Internal names written upper case without underscore assume the helpers are declared with
   the `pascal` keyword; compiling with another default convention changes the names.
4. VMAlloc output register EAX and VMLock output EAX = page count (source D only);
   the old `SelOffset` output of `DevHelp_VMAlloc` (not established, unused by HFS/2).
5. `parm loadds` on inline-code pragmas; whether Open Watcom's installed `h\os2` contains `devaux.h`.
6. Zero-filling of the IFS's uninitialised data by the kernel loader; `wlink` behaviour
   with no start address; which clib members are safe in ring 0.
7. Meaning of `pMiniFSD` for a non-boot IFS (not used by HFS/2).

## Exposure to forbidden material

- No file of the forbidden list was opened, read, searched, diffed or recovered:
  not `src\Ring0\fsd.h`, `fsh.h`, `src\Ring3\fsd.h`, `hfsflt\devhdr.inc`, `test.asm`,
  `doc\original\`, `r0struct.h`, anything under `C:\MiniDDK` or `C:\os2tk45`.
  One Bash command of mine named `fsd.h`/`fsh.h` in a `grep -l` and a `*.h` glob; it was
  refused by the permission system before running, so nothing was read. Subsequent searches used explicit file lists.
- IBM toolkit files are bundled in the public osfree-project/fat32 repository
  (`src/include/fsd.h`, `fsh.h`, `dhcalls.h`, `devhelp.h`, `src/ifsinf/`, binary
  `src/lib/fshelper.lib`, `dhcalls.lib`). They were seen only in the repository's file
  listing (names), NOT opened or downloaded. Only `src/ifs/fat32a.c`, `fat32b.c`, `helpers.asm`,
  `src/lib/makefile` and `fat32.def` were fetched. Recommendation: do not rely on that repository for any header.
  `fat32b.c` was downloaded but not read.
- reko's `src/Environments/OS2/16/fsh.inc` came up in a search for `FSH_PROBEBUF`; it was not opened
  (the previous pass noted that reko's OS/2 include files may descend from the IBM header).
- `github.com/OS2World/DEV-SAMPLE-DRV-FILESYS-WatcomIFS` was not downloaded or opened by me. The scratch folder
  of this session contains a directory named `watcomifs` that I did not create or open.
- `src\Ring0\c0.asm` (Borland start-up, copyright Borland, not in the forbidden list) exists in the tree; I printed its
  first 60 lines (banner and segment declarations) before deciding to read no more. Nothing was taken from it.
- Request to read import tables of installed `.IFS` binaries (`HFS.IFS`, `EXT2-OS2.IFS`) on this PC for a 16-bit
  confirmation was refused by the permission system and was not retried.
- No comment text from any source was copied; the wlink/wlib/pragma snippets are my own wording using documented syntax.
