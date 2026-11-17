# Clean-room specification: replacement headers for HFS/2

Status: SPEC v0.1, written 2026-11-17. Nothing in this file was copied from the
IBM-derived headers; every name below was collected from the GPL .c/.asm files of
HFS/2 (usage counts from `grep`), see "Sources of this list".

## 1. Why

`src/Ring0/fsd.h`, `src/Ring0/fsh.h`, `src/hfsflt/devhdr.inc` (and, partly,
`src/Ring3/fsd.h`) are derived from the IBM IFS Toolkit / DDK, which is not
GPL-compatible (see PLAN section 6). They must be replaced by headers written
without looking at the IBM text.

## 2. Rules for the implementer (clean-room)

You may read:
- this SPEC;
- the HFS/2 GPL source files, EXCEPT the forbidden files below;
- Open Watcom headers (`C:\WATCOM\h`, `h\os2`: `os2.h`, `bsedos.h`, `bsedev.h`, ... - their
  licence allows use);
- public documentation of the OS/2 IFS interface and the source of other GPL
  OS/2 file systems (for example JFS for OS/2, the ext2 IFS), as long as the file
  you read carries a GPL-compatible licence; record every document or file you used.

You must NOT open, read, search, diff or recover from git history:
- `src/Ring0/fsd.h`, `src/Ring0/fsh.h`, `src/Ring3/fsd.h`, `src/hfsflt/devhdr.inc`,
  `src/hfsflt/test.asm`, the same files in `doc/original/` or in git commits;
- anything under `C:\MiniDDK`, `C:\os2tk45`, any IBM DDK / IFS Toolkit header or include file;
- `src/Ring0/r0struct.h` lines marked "from IFS toolkit" (`OPDATA`, `INITSTRUC`).

Write original text: your own comments, your own ordering, no copied comment wording.
Field names, constant names and structure layout are functional facts of the kernel
interface and must match what the kernel expects; take them from public
documentation, not from the forbidden files.

If a layout, size or constant value cannot be established from allowed sources, do
NOT guess: write it with a `/* UNVERIFIED: <what is missing> */` comment and list it
in `PROVENANCE.md`. It will be checked later by a runtime test on the Dev VM.

## 3. Deliverables (put them in `doc/cleanroom/out/`)

1. `fsd.h`      - types, constants and prototypes of the file system driver side (Ring 0).
2. `fsh.h`      - prototypes and constants of the kernel helper (FSH_*) calls.
3. `devhdr.inc` - assembler constants for the device driver header (used by `segs.asm`).
4. `fsd_r3.h`   - the subset of the Ring 3 declarations that `src/Ring3/*.c` need
                  (replaces `src/Ring3/fsd.h`; derive it from the usage in the Ring 3 .c files).
5. `PROVENANCE.md` - for every file: which allowed sources were used, and the list of UNVERIFIED items.

Each new file starts with the GPL v2-or-later header used by HFS/2 and the line
"Clean-room replacement, written 2026 from public documentation; see PROVENANCE.md".

Must build with Open Watcom 2 (`-zK` mode for Ring 0, 16-bit `far pascal` conventions
as the existing code uses them) and keep the file names `fsd.h` / `fsh.h`
so they drop in (`#include "fsd.h"`). Include guards: use your own macro names.

## 4. What the HFS/2 code needs

### 4.1 Structure types used (`struct <tag>`, counts of uses in Ring0/Ring3 .c files)

| tag | uses | fields the code touches |
|---|---|---|
| vpfsd | 70 | (opaque to the code except as a pointer; HFS/2 stores its own volume data via it) |
| sffsi | 62 | sfi_position, sfi_size, sfi_h?, sfi_tstamp, sfi_mode, sfi_selfsfn, sfi_mtime, sfi_mdate, sfi_ctime, sfi_cdate, sfi_atime, sfi_adate |
| sffsd | 58 | (opaque, HFS/2 own per-file data) |
| cdfsd | 47 | (opaque) |
| cdfsi | 42 | cdi_curdir, cdi_h? |
| vpfsi | 36 | vpi_text, vpi_bsize, vpi_vid, vpi_drive, vpi_h? |
| fsfsd | 19 | (opaque) |
| fsfsi | 16 | fsi_h? |

(`?` = the name was seen as `xxx_h` in a grep and may be a handle-like field of another
name in the real structure: check the documentation.) The kernel fills the `*fsi`
structures and passes them to the driver; their layout must match the kernel. The
`*fsd` structures are driver-private work areas whose sizes the kernel fixes; the
HFS/2 definitions in `src/Ring3/fsdep.h` (GPL, by the original author) show the sizes the
driver already uses (volume 36, directory-cursor 8, file 30, search 24 bytes). Confirm
those sizes against the documentation; do not rely on them blindly.

### 4.2 FS_xxx entry points implemented by HFS/2 (signatures: see the definitions in
`src/Ring0/stubs.c`, e.g. FS_MOUNT, FS_ATTACH, FS_FSINFO, FS_FLUSHBUF, FS_CHDIR, ...)

FS_ALLOCATEPAGESPACE, FS_ATTACH, FS_CANCELLOCKREQUEST, FS_CHDIR, FS_CHGFILEPTR, FS_CLOSE,
FS_COMMIT, FS_COPY, FS_DELETE, FS_DOPAGEIO, FS_EXIT, FS_FILEATTRIBUTE, FS_FILEINFO,
FS_FILEIO, FS_FILELOCKS, FS_FINDCLOSE, FS_FINDFIRST, FS_FINDFROMNAME, FS_FINDNEXT,
FS_FINDNOTIFYCLOSE, FS_FINDNOTIFYFIRST, FS_FINDNOTIFYNEXT, FS_FLUSHBUF, FS_FSCTL,
FS_FSINFO, FS_INIT, FS_IOCTL, FS_MKDIR, FS_MOUNT, FS_MOVE, FS_NEWSIZE, FS_NMPIPE,
FS_OPENCREATE, FS_OPENPAGEFILE, FS_PATHINFO, FS_PROCESSNAME, FS_READ, FS_RMDIR,
FS_SETSWAP, FS_SHUTDOWN, FS_VERIFYUNCNAME, FS_WRITE.
Data exports used: `FS_NAME`, `FS_ATTRIBUTE`.

Provide prototypes for all of these in `fsd.h` (the existing definitions in
`stubs.c` / `r0comm.c` show the argument lists that compile today; the kernel
side of those argument lists must match the public documentation).

### 4.3 Constants used by the code

- FS attribute bits: FSA_REMOTE, FSA_UNC, FSA_LOCK, FSA_LVL7, FSA_ATTACH, FSA_ATTACH_INFO, FSA_DETACH (with FSD_ATTACH);
- FS_COMMIT_ONE, FS_COMMIT_ALL; FS_CL_ORDINARY, FS_CL_FORSYS, FS_CL_FORPROC;
- FSIL_ALLOC, FSIL_VOLSER (FS_FSINFO levels);
- PB_OPREAD, PB_OPWRITE (probe buffer operations);
- FILE_NORMAL, FILE_HIDDEN, FILE_DIRECTORY, FILE_CREATED, FILE_EXISTED;
- OPEN_ACCESS_READWRITE, OPEN_ACCESS_WRITEONLY, OPEN_ACTION_CREATE_IF_NEW,
  OPEN_ACTION_FAIL_IF_EXISTS, OPEN_ACTION_FAIL_IF_NEW, OPEN_ACTION_OPEN_IF_EXISTS,
  OPEN_ACTION_REPLACE_IF_EXISTS, OPEN_FLAGS_DASD (these may already come from the
  Open Watcom OS/2 headers: check there first and do not redefine them).

### 4.4 FSH_ helper calls used (call counts in HFS/2 sources)

FSH_SEMCLEAR 132, FSH_PROBEBUF 71, FSH_GETVOLPARM 31, FSH_SEMWAIT 7, FSH_SEMSET 6,
FSH_DOVOLIO 5, FSH_WILDMATCH 4, FSH_DEVIOCTL 4, FSH_UPPERCASE 3, FSH_SETVOLUME 3,
FSH_SEMREQUEST 3, FSH_SEGFREE 3, FSH_SEGALLOC 3, FSH_FINDDUPHVPB 3, FSH_YIELD 2,
FSH_STORECHAR 2, FSH_SEMSETWAIT 2, FSH_SEGREALLOC 2, FSH_REMOVESHARE 2,
FSH_REGISTERPERFCTRS 2, FSH_QSYSINFO 2, FSH_PREVCHAR 2, FSH_NAMEFROMSFN 2,
FSH_LOADCHAR 2, FSH_ISCURDIRPREFIX 2, FSH_IOBOOST 2, FSH_INTERR 2, FSH_GETPRIORITY 2,
FSH_GETOVERLAPBUF 2, FSH_FORCENOSWAP 2, FSH_FLUSHBUF 2, FSH_FINDCHAR 2, FSH_DOVOLIO2 2,
FSH_CRITERROR 2, FSH_CHECKEANAME 2, FSH_CANONICALIZE 2, FSH_ADDSHARE 2.

`src/Ring0/fshelper.c` (new code written for this port, NOT derived) implements these on
DevHelp: its function definitions show the argument lists that compile today and
are a valid reference for the prototypes. It also defines `SEM` as a typedef today;
decide where that typedef belongs.

### 4.5 Device driver header constants (for `devhdr.inc`)

`segs.asm` uses exactly: `DEVLEV_3`, `DEV_CHAR_DEV`, `DEV_ADAPTER_DD`. Provide those
(device level 3 attribute, character device bit, adapter device driver bit) with the
values given by the public documentation of the OS/2 device driver header
attribute word; add other attribute constants only if you can source them.
`src/hfsflt/hfsflt.h` also declares some constants locally ("from DDK"):
`DEVICECLASS_ADDDM`, `ERROR_I24_QUIET_INIT_FAIL`; re-source those too (they are
tiny, but confirm their values from allowed sources).

## 5. Acceptance

1. `compile_wat.cmd` builds with the new headers placed over `src\Ring0\fsd.h`, `fsh.h`,
   `src\Ring3\fsd.h` (replace by `fsd_r3.h`) and `devhdr.inc`, with no new errors;
2. `PROVENANCE.md` lists sources for every file and every UNVERIFIED item;
3. a grep of the new files for text from the forbidden files finds nothing (no copied
   comments, no "SCCSID", no IBM-internal path names);
4. a Dev VM load test of `hfs.ifs` (backup CONFIG.SYS first) confirms the layouts that
   were UNVERIFIED.

## 6. Sources of this list

`grep` over `src/` (excluding the forbidden files) for `FSH_*`, `FS_*`, `struct ...fsi/fsd`
and field-name patterns, on 2026-11-17. The call-count tables above come from that.
