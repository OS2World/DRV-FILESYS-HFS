# HFS/2

HFS/2 is an experimental OS/2 installable file system (IFS) for reading and
writing Macintosh HFS volumes. The package also contains a diskette filter, a
Ring 3 control program, a UHFS DLL (for FORMAT/CHKDSK), and utilities.

| Field | Details |
|---|---|
| Version | 0.20 (BLDLEVEL `@#OS2World:0.20#@`; the historical source release is 0.10) |
| Status | Partially working: everything builds; `hfs.ifs` loads on ArcaOS; mounting, listing, reading and writing files work on a generated HFS floppy image. The filter, the UHFS DLL (FORMAT/CHKDSK) and real Macintosh media are **untested**. |
| Tested on | ArcaOS 5.1.1 Dev VM (VirtualBox), 2026-11-17: IFS load, boot messages, control program, mount/dir/read/write on a floppy image made by `test\mkhfsimg.py`. No real hardware, no real Mac disks. |
| License | GNU GPL version 2 or later; see [LICENSE](LICENSE). See "License notes" below. |
| Authors | Marcus Better (original HFS/2, 1996-1997); OS2World (Open Watcom port, 16-bit IFS, clean-room headers, boot messages, 2026) |
| Original source | HFS/2 0.10 (April 12, 1997), Marcus Better; git history of https://github.com/OS2World/DRV-FILESYS-HFS (tag `0.10` = the original code). `libhfs` is Robert Leslie's. |
| Build tools | Open Watcom 2.0 (`wcc`, `wcc386`, `wasm`, `wlink`, `wlib`, `wmake`); MiniDDK headers and libraries for the filter and some constants; build with `compile_wat.cmd` |

## Build

Build from the project root on ArcaOS:

```text
compile_wat.cmd
```

`compile_wat.cmd` sets the Watcom environment, runs the root `Makefile.wat`,
checks that every expected output exists, prints an `OK:` or `FAILED:` line per
output with its size, and writes `compile.log` in the current directory.
`compile_wat.cmd clean` removes the outputs.

- `bin\hfs.ifs`: the Ring 0 IFS. **16-bit NE library module**, built with `wcc -ml`
  and linked with `wlink system os2 dll` (`protmode`, `stack=0`, `nodefaultlibs`,
  the `FS_*` exports of `src\Ring0\hfs_exports.lnk`, the kernel helper calls `FSH_*`
  imported by ordinal from `FSHELPER` through `doc\cleanroom\out2\fshelper_imports.lnk`).
  The kernel calls the entry points as 16-bit far routines; a 32-bit executable is refused.
- `bin\hfsflt.flt`: the diskette filter (16-bit BASEDEV driver).
- `bin\hfs.exe` and `fskill.exe`, `sync.exe`, `readres.exe`, `bindisk.exe`, `prepare.exe`:
  32-bit flat programs (`wcc386 -mf -s`).
- `dll\uhfs.dll`, `lib\libhfs16.lib`, `lib\libhfs32.lib`.
- `bin\obj\<component>\`: objects and map files.

The makefiles expect Open Watcom in `C:\WATCOM` and MiniDDK in `C:\MiniDDK`.

## Install

1. Back up `CONFIG.SYS` and know how to restore it: a bad driver can stop the boot.
2. Copy `HFS.IFS` to a directory (for example `C:\HFS`) and add the line
   `IFS=C:\HFS\HFS.IFS [parameters]`. It must come **after** the `IFS=` line of the
   file system that holds the file (for example after `JFS.IFS`).
3. Start the control program: `RUN=C:\HFS\HFS.EXE`, or from `STARTUP.CMD`
   (`detach C:\HFS\HFS.EXE`). The IFS does nothing without it.
4. Optional filter for diskettes: copy `HFSFLT.FLT` to `\OS2\BOOT` and add
   `BASEDEV=HFSFLT.FLT [parameters]`. Untested; do not use it on a system you cannot recover.
5. Long instructions (formatting, CHKDSK, file name translation): `doc\hfs.ipf`
   (view with `view doc\hfs.inf` after compiling the IPF, or read the 1997 `doc\README.txt`).

## Parameters and boot messages

Boot messages follow the EDM2 "Suggestion for OS/2 and eComStation Drivers
Arguments":

- Quiet by default: only errors are shown.
- Every line starts with `HFS: `; long messages are word-wrapped at 79 columns and
  every line gets the prefix.
- Errors read `HFS: HFS/2 could not be loaded. <reason>`.

| Parameter | `HFS.IFS` | `HFSFLT.FLT` | `HFS.EXE` |
|---|---|---|---|
| `/V` | version and build date | version and build date | version and build date |
| `/Q` | no messages at all, not even errors | same | no startup banner or diagnostics |
| `/W` | wait for a key after the messages | not supported (see below) | - |
| `/AA` | beep when the driver loaded | not supported (see below) | - |

`/W` and `/AA` are not available in `HFSFLT.FLT` because a BASEDEV driver runs before the
file and keyboard services they need exist. `HFS.EXE` also accepts `/FN:mode`
(`CP850`, `7BIT`, `8BIT`: file name translation) and `/NOSYNC`.

Example (verified on the Dev VM): `IFS=C:\HFS\HFS.IFS /V /W /AA` prints
`HFS: Version 0.20 (built ...)` and `HFS: HFS/2. Press any key to continue.`

## BLDLEVEL

All binaries carry `@#OS2World:0.20#@` plus a description (`C:\OS2\BLDLEVEL.EXE bin\hfs.ifs`):
`hfs.ifs`, `hfsflt.flt`, `uhfs.dll`, `hfs.exe`, `fskill.exe`, `sync.exe`, `readres.exe`,
`bindisk.exe`, `prepare.exe`. The IFS banner version comes from `src\include\banner.h`.

## Sample / test

`sample\` is empty. `test\mkhfsimg.py` (Python 3) writes a 1.44 MB HFS floppy image with a few files and
folders and an OS/2 boot sector (a raw Mac diskette is not recognised by OS/2 without the filter driver or
`PREPARE`). Test used on the Dev VM: load the IFS, start `HFS.EXE` detached, attach the image to the VM
floppy drive (change the volume serial in the image each time, otherwise the kernel treats it as the
same disk re-inserted), then `dir A:`, `type`, `copy` in both directions, `SYNC` before removing it.

## Known issues

- The filter, the UHFS DLL and real Macintosh media have not been tested. The value used for `ERROR_VOLUME_NOT_MOUNTED` (26) is a placeholder that could
  not be sourced; it matters when `FS_MOUNT` rejects a volume (check on the VM that a non-HFS
  volume still falls through to the other file systems without an error popup).
- `FSKILL.EXE` returned success but the control program kept running in one test.
- Time stamps shown by `dir` are off by several hours; `dir` shows "0 bytes free" although the volume
  reports free blocks; the alternate MDB is not updated by writes.
- The compiler gives about 60 warnings (pointer and type conversions) that need review.
- Four layout details of the clean-room headers are marked UNVERIFIED
  (`doc\cleanroom\out\PROVENANCE.md`).

## License notes

The source is GPL v2 or later. The Ring 0 and Ring 3 headers that were derived from the IBM
IFS Toolkit / DDK were replaced by clean-room headers (`doc\cleanroom\`). Building
`hfsflt.flt` still links libraries from the IBM DDK (`dhcalls.lib`, `addcalls.lib` from
MiniDDK), and the DDK headers are used at build time; MiniDDK is **not** part of this
repository and may not be redistributed. Check the IBM DDK licence before distributing
binaries of the filter.

## Links

- https://github.com/OS2World/DRV-FILESYS-HFS
- `doc\hfs.ipf`: original documentation; `doc\README.txt`: the 1997 readme
- `doc\cleanroom\SPEC.md`: how the replacement headers were written

## Changelog

Newest first; details in [doc/CHANGES.md](doc/CHANGES.md).

- 0.20: Open Watcom build (`compile_wat.cmd`); Ring 0 IFS rebuilt as a 16-bit module that loads
  on ArcaOS; clean-room headers replace the IBM-derived ones; standard boot messages and
  parameters (`HFS:` prefix, `/V /Q /W /AA`); BLDLEVEL on all binaries.
- 0.10: original release by Marcus Better (1997).
