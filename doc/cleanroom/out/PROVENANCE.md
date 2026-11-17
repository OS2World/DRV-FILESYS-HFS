# Provenance of the clean-room headers

Written 2026-11-17 against `doc/cleanroom/SPEC.md` v0.1. Nothing was built or
compiled (no Open Watcom on the machine used, and no compiler at all), so none
of the headers has been through a compiler yet.

## Sources used

| # | Source | Licence | Used for |
|---|---|---|---|
| S1 | HFS/2 GPL sources: `src/Ring0/stubs.c`, `r0comm.c`, `fshelper.c`, `r0struct.h`, `src/Ring3/*.c`, `fsdep.h`, `src/include/r0r3shar.h`, `hfsctl.h`, `src/hfsflt/hfsflt.h`, `segs.asm`, `Makefile.wat` files | GPL v2+ | argument lists of every FS_/FSH_ function, which fields and constants are touched, work-area sizes, where `SEM` lives |
| S2 | JFS for OS/2, as published in `https://github.com/osfree-project/openjfs`: `src-other/jfs/common/include/fsd.h`, `fsh.h` (raw files fetched from raw.githubusercontent.com) | GPL v2 or later (repository licence GPL-2.0; file headers carry an IBM copyright plus the GPL grant) | field names/order/sizes of vpfsi, cdfsi, sffsi, fsfsi, work areas, FS_FSCTL/NMPIPE/DOPAGEIO records; numeric values of all flag groups; FSH prototypes and flag values |
| S3 | Open Watcom DD headers as shipped in `C:\Temporal\1.- OS2\Projects\Drivers\uniaud16\owddk\` (`bsedos.h`, `devhdr.h`, `devhelp.h`, `os2def.h`, `bseerr.h`); file headers say "Open Watcom Project", Copyright The Open Watcom Contributors | Open Watcom Public License | `CCHMAXPATH`=260, `FILE_*`/`OPEN_*`/`FSIL_*`/`FSCTL_FSDNAME` exist there (so not redefined), `FILELOCK` layout, device attribute word bit values (`DAW_*`), capability bit `CAP_ADD`, `DEVICECLASS_ADDDM`=1 |

`C:\WATCOM\h` does not exist on this PC; S3 (a copy of Open Watcom's own
headers inside another project) was used instead. Re-check against the real
`C:\WATCOM\h\os2` on the build VM.

No IBM IFS Toolkit/DDK header, MiniDDK or os2tk45 file was opened.

### Caution on S2
The two JFS headers carry an IBM copyright notice and an SCCSID line, and
their structure looks like it descends from IBM's IFS interface header. They
are published by IBM under GPL v2+, and SPEC section 2 names JFS for OS/2 as an
allowed source, so they were used. Only structure facts (names, order, sizes,
numeric values) were taken; the text, ordering, comments and layout of the new
files are original. If the project owner judges that S2 is too close to the
proprietary header, every item marked "S2" below has to be re-sourced.

## Per file

### fsd.h
- Structures and constants: S2 (values), cross-checked against actual field use in S1.
- Work-area sizes 36 / 8 / 30 / 24: S2 and S1 (`Ring3/fsdep.h`) agree.
- FS_xxx prototypes: S1 (`stubs.c`, `r0comm.c`); kept identical, including `unsigned far *` in FS_IOCTL.
- `FS_VERIFYUNCNAME`: no Ring 0 definition exists; argument list from the Ring 3 definition in S1 (`Ring3/misc.c`).
- Calling-convention words (`far pascal _export _loadds`) are used as in S1; not defined here.
- `FILE_*`, `OPEN_*`, `FSIL_*`, `FSCTL_FSDNAME`: not defined, supplied by S3 / the real Open Watcom `os2.h`.
- `vpi_pDCS`/`vpi_pVCS` are carried as raw dwords (no code touches them).
- `INFO_RETREIVE` kept as an alias because Ring 3 code spells it both ways.
- FS_FSCTL pointer members use `far *` because `r0comm.c` dereferences them.

### fsh.h
- Prototypes: argument lists of S1 `fshelper.c` (compile today); parameter order and flag values cross-checked with S2.
- `FSH_SETVOLUME`: not in S2; from S1 `fshelper.c` only.
- `SEM` typedef is NOT in this file: semaphore parameters are `unsigned long far *` (same type) so that `r0struct.h` stays the single definition (a duplicate typedef may not compile).
- `FSH_PROBEBUF` takes `void far *` to match S1's definition (S2 uses `char far *`).

### devhdr.inc
- `DEV_CHAR_DEV` 8000h, `DEVLEV_3` 0180h, other attribute bits: S3 `devhdr.h` (`DAW_CHARACTER`, `DAW_LEVEL3`, ...).
- `DEV_ADAPTER_DD` 08h: S3 `CAP_ADD` (see UNVERIFIED U9).

### fsd_r3.h
- Derived from the Ring 3 .c files in S1 (definitions of the FS_/FSD_ functions and the constants they use); structure layouts duplicated from fsd.h so they match for `r0r3shar.h`.
- Does not define vpfsd/cdfsd/sffsd/fsfsd (Ring 3 `fsdep.h` owns them).

## UNVERIFIED items (to be settled by the VM load test)

| # | Item | Where | Note |
|---|---|---|---|
| U1 | Whether the kernel's `sffsi` on the target ArcaOS includes the trailing large-file members `sfi_pPVDBFil`, `sfi_DOSattr`, `sfi_sizel`, `sfi_positionl` (total size) | fsd.h, fsd_r3.h | S2 has them (32-bit compile); no allowed source states which kernel versions pass them. HFS/2 never reads them. |
| U2 | Width of pointers inside kernel structures: `struct SF`, `struct CD`, `npx_obuf`, `npt_obuf` use `far *`. With the Ring 0 build (`wcc386 -ml -zK`) a `far *` may be 16:32, not the kernel's 16:16 | fsd.h | S2 uses `_Seg16` 16:16 pointers. Check `sizeof(struct SF)` = 8 and how `far` is mapped. |
| U3 | `struct filelock` = {long lOffset; long lRange;} | fsd.h, fsd_r3.h | Taken from the user-level lock record (S3); not confirmed that the FS_FILELOCKS record is the same. |
| U4 | `ERROR_I24_QUIET_INIT_FAIL` (0x8018 in hfsflt.h) | hfsflt.h | No allowed source other than hfsflt.h itself; also note the macro there lacks parentheses. Not changed. |
| U5 | `DEVICECLASS_ADDDM` = 1 | hfsflt.h | Confirmed by S3 `devhelp.h`; listed only for completeness (no change needed). |
| U6 | Meaning of `fControl` of `FSH_SETVOLUME` and its prototype | fsh.h | From S1 only. No flag constants defined. |
| U7 | `FS_NMPIPE` record sizes (`short`/`long` in 16-bit packing) | fsd.h | S2, 32-bit compile; unused by HFS/2 beyond the type name. |
| U8 | `FS_FILELOCKS`/`FS_CANCELLOCKREQUEST` take `void far *` (as defined in `stubs.c`), but `stubs.c` dereferences them (`= *pLockRange`), which is invalid C for `void`. | fsd.h | Pre-existing in S1, not introduced here. Suggested follow-up: change the definitions to `struct filelock far *`. |
| U9 | `DEV_ADAPTER_DD` = 00000008h: identified with `CAP_ADD` ("participates in ADD strategy") by meaning, because the name is not in any allowed source | devhdr.inc | Confirm that the third dword of the header must hold 8 for the filter to load. |
| U10 | How `far`, `pascal`, `_export`, `_loadds` are provided under `-zK` | fsd.h, fsh.h | The headers rely on them being available, as the .c files already do; not checkable without the compiler. |
| U11 | Which `INCL_*` macros must be set so `os2.h` supplies `FILE_*`, `OPEN_*`, `FSIL_*`, `FSCTL_*` | all | Existing .c files define INCL_DOSFILEMGR / INCL_DOSERRORS; check `stubs.c` and `r0comm.c` (FSIL_* and OPEN_FLAGS_DASD need the file-manager group, FSCTL_FSDNAME the device group, per S3). |

## Not-done / follow-ups
- Not compiled; acceptance step 1 (`compile_wat.cmd`) is still open.
- No comment text, SCCSID or path names from the proprietary headers were copied (checked by grep for "SCCSID", "@(#)" and path fragments).
- The project's `stubs.c` also needs `struct filelock` usage only through `r0r3shar.h`; it is provided in both headers.

## Syntax-only fixes added after the first build (2026-11-17, by the supervising session)
These carry no kernel interface information; they only make the headers usable with Open Watcom 2.0.1 `wcc386 -zK`.
1. New file `hfs_cc.h`, included by `fsd.h` and `fsh.h`: `far` and `near` expand to nothing and `pascal` to `_System` for 32-bit Watcom, because `-zK` turns those words into plain identifiers
   while the Ring 0 sources still spell their entry points with them. Written from scratch (own text). Found by the first build: ~70 "invalid declarator" errors at the entry point prototypes.
2. `fsd.h`: `#undef FS_ATTACH` before the entry point prototypes. The OS/2/DDK headers define `FS_ATTACH` as a plain number; it collides with the entry point name. Found by compiling a test file that
   checked every FS_* name for a macro definition (only FS_ATTACH is one). The Ring 0 code does not use the macro's value.
Result of the scratch build after these two fixes: Ring 0 has 1 error left, the missing constant below.

## Open item found by the first build (NOT fixed here, needs an allowed source)
- `ERROR_VOLUME_NOT_MOUNTED` is used by `src/Ring0/stubs.c` and `src/Ring3/vol.c` and is declared nowhere (not in the new headers, not in Open Watcom's `bseerr.h`). Its value must be established from public
  documentation or a GPL-compatible source and added (probably to `fsd.h` and `fsd_r3.h`, guarded with `#ifndef`).

## Update 2026-11-17 (second clean-room pass: missing constant and open items)

Only files in `doc/cleanroom/out/` were changed: `fsd.h`, `fsd_r3.h`, `PROVENANCE.md`.
No forbidden file was opened (the HFS source was searched with explicit file lists).

### Additional sources used
| # | Source | Licence | Used for |
|---|---|---|---|
| S4 | VirtualBox shared-folder FSD for OS/2, `src/VBox/Additions/os2/VBoxSF/VBoxSFFile.cpp` (https://github.com/VirtualBox/virtualbox, fetched through raw.githubusercontent.com) | GPL-3.0-only or CDDL (dual); only facts (member names, use of 64-bit members) were taken, no text | existence of `sfi_sizel`, `sfi_positionl`, `sfi_DOSattr` in the kernel's open-file record (32-bit FSD, FS32_ entry points) |
| S5 | Reko decompiler OS/2 16-bit environment, `src/Environments/OS2/16/fsd.inc` (https://github.com/uxmal/reko) | GPL-2.0-or-later | cross-check that an older 16-bit record lacks `vpi_flags` and the `sffsi` tail. NOTE: this file may itself descend from the IBM header (its comments look like it); it was used ONLY as a yes/no check, nothing copied |
| S6 | Open Watcom v2, `bld/os2api/incl32/bseerr.mh`, `incl32/os2def.mh`, `incl16/bseerr.mh` (https://github.com/open-watcom/open-watcom-v2, master) | Open Watcom Public License | `ERROR_I24_QUIET_INIT_FAIL` = 21; `LONGLONG` = {ulLo, ulHi}; `MAKEP`/`MAKE16P` = (sel << 16) or off; absence of `ERROR_VOLUME_NOT_MOUNTED` |
| S7 | osFree, `tools/UNI2/UNI/inc/basemid.inc`, `tools/UNI2/UNI/os2/cpi/basemid.uni`, `OS2/WPS/contrib/src/xwpsec_ring0/strat_init_base.c` (https://github.com/osfree-project/osfree) | osFree is mostly GPL-2.0-or-later / BSD-style per file; used for numeric facts only | second source for `ERROR_I24_QUIET_INIT_FAIL` = 21 (also used there as `STERR + 21`); complete OS/2 error-number list searched for the missing name |
| S8 | VirtualBox `src/VBox/HostDrivers/VBoxUSB/os2/usbcalls.c` (same repository as S4) | GPL-3.0-only or CDDL | third source: `#define ERROR_I24_QUIET_INIT_FAIL 21` |
| S9 | Local Open Watcom DD headers copy (S3 above), `os2def.h` | Open Watcom Public License | `MAKEP`, `SELECTOROF`, `OFFSETOF`, `MAKEULONG` and `LONGLONG` layout |
| S10 | osfree-project/openjfs JFS headers and `jfs_ifs.c` (S2 above, all .h/.c under `src-other/jfs` searched for the error name and read for FS32_MOUNT return codes) | GPL v2 or later | checked: JFS does not use the missing error name either |

Source search used the public anonymous Sourcegraph stream API (sourcegraph.com) to locate files, then the raw files were read.

### Item 1: missing constant and scan for others
- `ERROR_VOLUME_NOT_MOUNTED` (U12): NOT SETTLED. It is absent from Open Watcom's `bseerr.h` (16 and 32 bit), from osFree's complete `basemid` error list, from the JFS sources, from kLIBC, and a literal search of public code hosting found no OS/2 header defining it. Most likely it was defined by the proprietary IFS header that this task replaces, which cannot be consulted. Added to `fsd.h` and `fsd_r3.h` under `#ifndef` with the PLACEHOLDER value 26 (= `ERROR_NOT_DOS_DISK`, value from S6) so the build proceeds. This is a guess and is marked UNVERIFIED in the header. Where it matters: `FS_MOUNT` returns it when the volume is rejected (`stubs.c`, `Ring3/vol.c`); the kernel might act on the exact code. Test: mount a non-HFS volume on the Dev VM and confirm the kernel falls through to other FSDs and no error popup appears.
- Scan of identifiers beginning `ERROR_`, `FSH_`, `FS_`, `FSA_`, `FSD_`, `CDI_`, `SFI_`, `VPI_`, `FSIL_`, `FSCTL_`, `PB_` in the allowed Ring 0 / Ring 3 sources (148 distinct names), compared with the new headers: not defined by the new headers are only
  - 26 `ERROR_*` names: ACCESS_DENIED, BAD_ARGUMENTS, BAD_FORMAT, BUFFER_OVERFLOW, CANNOT_COPY, CANNOT_MAKE, DIRECTORY, DIR_NOT_EMPTY, DISK_FULL, FILE_EXISTS, FILE_NOT_FOUND, GEN_FAILURE, INSUFFICIENT_BUFFER, INTERRUPT, INVALID_CATEGORY, INVALID_LEVEL, INVALID_PARAMETER, NOT_READY, NOT_SUPPORTED, NO_MORE_FILES, OPEN_FAILED, PATH_NOT_FOUND, READ_FAULT, SEM_TIMEOUT, WRITE_FAULT, WRITE_PROTECT (all present in the Open Watcom `bseerr.h`, S3/S6, with `INCL_DOSERRORS`) and `ERROR_NOT_FOUND` (defined as 1168 locally and guarded in `fshelper.c`, so not needed here);
  - `FSIL_ALLOC`, `FSIL_VOLSER` (present in Open Watcom `bsedos.h`, S3: 1 and 2);
  - 12 `FSCTL_FUNC_*` names (defined in `src/include/r0r3shar.h`, which is allowed);
  - `FSH_` alone (a comment fragment).
  No other missing constant or type was found. Other identifier families (for example `NO_ERROR`, `FILE_*`, `OPEN_*`, structure types from `os2.h`) were not part of the requested grep; the supervisor's rebuild remains the check for those.

### Item 2: the four UNVERIFIED items
| Item | Result | Evidence |
|---|---|---|
| `vpi_flags` (U13) | Layout and flag values supported by one source (JFS, S2); an older 16-bit record (S5) ends at `vpi_unit`, so the member was added later. HFS/2 never reads or writes `vpi_flags`, `VPB_*`, `vpi_pDCS` or `vpi_pVCS` (grep of all allowed files). Kernel builds that pass it remain UNDOCUMENTED, so the comment is kept as UNVERIFIED but it is harmless (only `sizeof` is affected). | S2, S5, S1 grep |
| `sffsi` large-file tail (U1) | Existence, member names and order of `sfi_DOSattr`, `sfi_sizel`, `sfi_positionl` confirmed by S2 and S4; width 8 bytes each, low word first (S6, S9); older 16-bit record (S5) ends at `sfi_pPVDBFil`/`sfi_DOSattr` without them. Not settled: which kernel builds hand the tail to a 16-bit FSD. HFS/2 never reads or writes any of them (grep). Kept as "UNVERIFIED (partly)". | S2, S4, S5, S6, S9 |
| `ERROR_I24_QUIET_INIT_FAIL` (U4) | SETTLED = 21 (decimal, 0x15) by four sources: S6 (Open Watcom), S7 (osFree, two files), S8 (VirtualBox). The macro in `src/hfsflt/hfsflt.h` (`0x8000 | 0x0018`, i.e. 0x8018, unparenthesised) does NOT match. See finding below. Not changed here (hfsflt.h is outside `out/`). | S6, S7, S8 |
| 8-byte integer order / 16:16 pointer order (U2 part) | SETTLED: 64-bit quantity = low dword first then high dword (`LONGLONG` = {ulLo, ulHi}, S6/S9); a 16:16 pointer in a dword has the offset in the low word and the selector in the high word (`MAKEP(sel, off)` = (sel << 16) or off, S6/S9). Both already match `struct hfs_quad` and the raw-dword treatment of `vpi_pDCS`/`vpi_pVCS`. Still open from U2: how `far *` is compiled under `-zK` inside `struct SF`/`struct CD` (needs sizeof test on the VM; unrelated to the order). | S6, S9 |

Finding for the project owner (not fixed, outside `out/`): in `src/hfsflt/init.c` the status is built as `STDON+STERR+ERROR_I24_QUIET_INIT_FAIL`. With the current macro text the expression is `(STDON+STERR+0x8000) | 0x0018`, which overflows the 16-bit status word and drops the error bit, and the low byte 0x18 is not the code 21 the public headers give. The intended value is probably `STDON + STERR + 21` (= 0x8115). Suggest replacing the macro by `#define ERROR_I24_QUIET_INIT_FAIL 21` and testing that a failed filter load reports an error.
