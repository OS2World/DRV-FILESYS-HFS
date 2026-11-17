# Changes from the original HFS/2 source

- `src\`: source trees are organized by component (Ring-0 IFS, Ring-3 control
  program, filter driver, UHFS DLL, and utilities). Original build recipes are
  retained under `doc\original\`.
- `Makefile.wat`, component `Makefile.wat` files, `compile_wat.cmd`: build the
  complete project from the root. Executables and driver images are emitted in
  `bin\`, objects in component-specific `bin\obj\` directories, the UHFS DLL in
  `dll\`, and the 16-bit and 32-bit libraries in `lib\`. The build log is
  written to the caller's current directory.
- `src\include\`: place shared headers in the common include directory used by
  the component builds.
- `src\Ring0\r0comm.c`, `src\include\banner.h`: make successful IFS
  initialization quiet by default, add `/V`, make `/Q` suppress error messages,
  prefix IFS boot messages, and omit copyright text from the boot banner.
- `src\Ring3\r3comm.c`: make the control program's version banner opt-in through
  `/V`, suppress startup diagnostics with `/Q`, and prefix startup errors.
- `src\hfsflt\init.c`: remove the unconditional copyright banner and prefix the
  filter's initialization failure messages.
- `doc\hfs.ipf`: document the IFS `/V` and `/Q` options.
- `README.md`, `LICENSE`, `.gitignore`: document the source, GPL terms, build,
  supported parameters, and generated-file policy.
- `doc\file_id.diz`: describe the driver as experimental and note that current
  runtime behavior has not been verified.
- `src\Ring0\Makefile.wat`, `src\hfsflt\Makefile.wat`, `src\UHFS\Makefile.wat`:
  embed BLDLEVEL metadata with vendor `OS2World` and version `0.20` in the
  loadable IFS, filter, and UHFS DLL.

## Build verification

On 2026-11-15, the full `compile_wat.cmd` build completed on the ArcaOS Dev VM
with Open Watcom Make 2.0.1. The build produced the expected images, executable
utilities, objects, DLL, and libraries in the directories listed in the
README. IBM's BLDLEVEL utility verified the selected vendor, version, and
descriptions on `hfs.ifs`, `hfsflt.flt`, and `uhfs.dll`. The build has
non-fatal warnings; the driver was not installed or loaded, so runtime
compatibility remains unverified.

The source tree had already been relocated before this work began, so no
pre-relocation baseline build was available.

## Clean-room headers (2026-11-17)

- `src\Ring0\fsd.h`, `src\Ring0\fsh.h`, `src\Ring3\fsd.h`, `src\hfsflt\devhdr.inc`:
  replaced by clean-room headers (`src\Ring0\hfs_cc.h` is new). The previous
  files were derived from the IBM IFS Toolkit / DDK. Sources, licences and the
  list of unverified items: `doc\cleanroom\out\PROVENANCE.md`; method and rules:
  `doc\cleanroom\SPEC.md`.
- `src\hfsflt\hfsflt.h`: `ERROR_I24_QUIET_INIT_FAIL` is now 21; the old
  `0x8000 | 0x0018` had no parentheses and dropped the error bit in
  `STDON+STERR+...`.

## Ring 0 IFS is now a 16-bit module (2026-11-17)

The kernel calls the `FS_*` entry points as 16-bit far pascal routines. The
32-bit flat executable produced before was refused by ArcaOS ("SYS1719 ... does
not contain a valid device or file system driver").

- `src\Ring0\Makefile.wat`: compile with the 16-bit `wcc` (large model, `-zu -s`)
  and link as an NE library (`system os2 dll`, `protmode`, `stack=0`,
  `nodefaultlibs`) with the exports of `src\Ring0\hfs_exports.lnk`.
- `doc\cleanroom\out2\fshelper_imports.lnk`: the kernel helper calls `FSH_*` are
  imported by ordinal from module `FSHELPER` (clean-room; see `PROVENANCE2.md`).
- `src\Ring0\devhelp_ifs.asm`, `dh16.h`: DevHelp calls through the entry point
  that `FS_INIT` receives (replaces `dhcalls.lib`).
- `src\Ring0\hfs_rt.c`: `memcpy`, `strncpy`, `strlen`, `strcpy`, `toupper`
  (the C library versions pull in the run-time start-up and stack checker).
- `src\include16\os2.h`: shadow header adding `PPVOID` for the DDK headers.
- `src\Ring0\r0comm.c`: removed the `Startup()` call; `src\Ring0\stubs.c`,
  `r0comm.c`: `far` added to the `pvpfsi`/`pvpfsd` locals so they match the
  `FSH_GETVOLPARM` prototype.
- Removed from the build and kept in `doc\original\port-32bit-attempt\`: the 32-bit
  `startup.asm`, `startup.c`, `fshelper.c` (a DevHelp emulation of the `FSH_*`
  calls, some of them stubs) and the 32-bit `Makefile.wat`.

Load test, ArcaOS Dev VM, 2026-11-17: with `IFS=C:\HFS\HFS.IFS /V` in CONFIG.SYS
the IFS is accepted, `FS_INIT` runs and prints "HFS/2: Version 0.10 (built ...)",
and the system boots to the desktop. The control program, the filter and
mounting a volume have NOT been tested; the `ERROR_VOLUME_NOT_MOUNTED` value
(26) is a placeholder (see `PROVENANCE.md`).

## Ring 3 programs: flat memory model (2026-11-17)

- `src\Ring3\Makefile.wat`, `src\Ring3\libhfs\Makefile.wat`, `src\util\Makefile.wat`, `src\util\Prepare\Makefile.wat`:
  compile with `-mf` (flat) instead of `-ml` and with `-s` (no stack checks); `hfs.exe` gets a 256 KB stack.
  The 32-bit programs built with the segmented large model crashed at start (DS = 0, or "Stack Overflow!").

Test, ArcaOS Dev VM, 2026-11-17 (IFS loaded as above): `HFS.EXE /?` prints its usage; `HFS.EXE /V` started
detached connects to the IFS without a trap (a second instance reports "Failed to connect to the IFS ...
no other HFS.EXE control program is running"); `FSKILL.EXE` returns 0 but the control program kept running.
No HFS volume has been mounted yet (no HFS media was available).

## Version 0.20: boot messages and BLDLEVEL (2026-11-17)

- All boot messages use the prefix `HFS: ` (IFS, control program, filter; the filter used `HFSFLT:`).
  Error form `HFS: HFS/2 could not be loaded. <reason>`; long messages are word-wrapped at 79 columns with
  the prefix on every line (`HfsMsg()` in `src\Ring0\r0comm.c`).
- `src\Ring0\r0comm.c`: new options `/W` (wait for a key) and `/AA` (beep) for HFS.IFS, `FS_INIT` ends through
  `InitDone()`. Verified on the Dev VM: `/V /W /AA` (version line, then "Press any key to continue."), `/Z`
  ("Invalid parameters." error) and `/Q` (no output).
- `src\hfsflt\init.c`: parses `/V` and `/Q` from the BASEDEV line, prints a version line with `/V`, `/Q`
  silences the error messages. `/W` and `/AA` are not available in a BASEDEV driver. NOT tested on the VM.
- `src\include\banner.h`: `HFS_VERSION` / `HFS_BANNER`, version 0.20 (was 0.10 in the banner).
- BLDLEVEL `@#OS2World:0.20#@` plus a description on `hfs.exe`, `fskill.exe`, `sync.exe`, `readres.exe`,
  `bindisk.exe`, `prepare.exe` (the IFS, filter and DLL already had it); the utility links now use response files.
- `README.md` rewritten to the project template; `doc\hfs.ipf`: options and a 0.20 history entry;
  `doc\file_id.diz`: version 0.20.

## Mount test and fixes (2026-11-17)

Tested on the ArcaOS Dev VM with a floppy image generated by `test\mkhfsimg.py` (new): mount, `dir`, `type`,
reading a 5000-byte file (byte-identical), creating a folder and writing two files (checked from the host: file data,
free-block count and allocation bitmap consistent).

- `src\Ring3\libhfs\Makefile.wat`: `-dIFS`. Without it the library contained the file-based `hfs_mount`, and the
  control program crashed with a volume handle used as a pointer.
- `src\Ring3\Makefile.wat`: `-zp1` (byte packing). The 32-bit OS/2 info structures are padded (FSALLOCATE 20 bytes
  instead of the 18 the kernel passes), which made `FS_FSINFO` fail with a buffer overflow.
- `test\mkhfsimg.py`: new generator for the test image; `.gitignore` ignores `test\*.img`.
