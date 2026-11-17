/*
 * HFS/2 - A Hierarchical File System Driver for OS/2
 * GPL v2 or later.  Written 2026 for the Open Watcom port.
 *
 * dh16.h - DevHelp calls used by the Ring 0 code of the 16-bit IFS build.
 * They are implemented in devhelp_ifs.asm (no dhcalls.lib: an IFS gets its
 * DevHelp entry point through FS_INIT).  Include AFTER <dhcalls.h>: the macros
 * below send the old DevHelp_xxx call sites to these routines.
 */

#ifndef HFS2_DH16_H_INCLUDED
#define HFS2_DH16_H_INCLUDED

unsigned short far pascal HFS_VMAlloc(unsigned long flags, unsigned long size,
                                      unsigned long phys, unsigned long far *lin,
                                      void far *unused);
unsigned short far pascal HFS_VMLock(unsigned long flags, unsigned long lin,
                                     unsigned long size, unsigned long pagelist,
                                     unsigned long lockhandle,
                                     unsigned long far *pagecount);
unsigned short far pascal HFS_VMUnLock(unsigned long lockhandle);
unsigned short far pascal HFS_LinToGDTSelector(unsigned short sel,
                                               unsigned long lin,
                                               unsigned long size);
unsigned short far pascal HFS_AllocGDTSelector(unsigned short far *sels,
                                               unsigned short count);

#define DevHelp_VMAlloc           HFS_VMAlloc
#define DevHelp_VMLock            HFS_VMLock
#define DevHelp_VMUnLock          HFS_VMUnLock
#define DevHelp_LinToGDTSelector  HFS_LinToGDTSelector
#define DevHelp_AllocGDTSelector  HFS_AllocGDTSelector

#endif /* HFS2_DH16_H_INCLUDED */
