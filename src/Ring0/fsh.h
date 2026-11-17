/*
 * HFS/2 - A Hierarchical File System Driver for OS/2
 * Copyright (C) 1996, 1997 Marcus Better
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Clean-room replacement, written 2026 from public documentation; see PROVENANCE.md
 */

/* fsh.h  (Ring 0)
 *
 * Prototypes and flag values for the kernel helper services (FSH_xxx) that a
 * file system driver imports, plus the MFSH_xxx variants used by the mini
 * file system.  All of them are 16-bit far pascal routines.
 *
 * Semaphore arguments are declared as pointers to an unsigned long (the
 * "SEM" type of r0struct.h); the typedef itself is deliberately NOT repeated
 * here so that r0struct.h stays the single place that defines it.
 *
 * struct vpfsi / struct vpfsd are declared in fsd.h; include that first.
 */

#ifndef HFS2_RING0_FSH_H_INCLUDED
#define HFS2_RING0_FSH_H_INCLUDED

#include "hfs_cc.h"

/* ------------------------------------------------------------------ */
/* Flag and value groups                                                */
/* ------------------------------------------------------------------ */

/* time-out arguments of the semaphore helpers, milliseconds */
#define TO_INFINITE  0xFFFFFFFFUL
#define TO_NOWAIT    0x00000000UL

/* FSH_PROBEBUF operation */
#define PB_OPREAD    0x0000
#define PB_OPWRITE   0x0001

/* FSH_DOVOLIO operation bits */
#define DVIO_OPREAD     0x0000
#define DVIO_OPWRITE    0x0001
#define DVIO_OPBYPASS   0x0002
#define DVIO_OPVERIFY   0x0004
#define DVIO_OPHARDERR  0x0008
#define DVIO_OPWRTHRU   0x0010
#define DVIO_OPNCACHE   0x0020
#define DVIO_OPRESMEM   0x0040

/* FSH_DOVOLIO allowed-reaction bits */
#define DVIO_ALLFAIL    0x0001
#define DVIO_ALLABORT   0x0002
#define DVIO_ALLRETRY   0x0004
#define DVIO_ALLIGNORE  0x0008
#define DVIO_ALLACK     0x0010

/* FSH_CRITERROR allowed-reaction bits */
#define CE_ALLFAIL      0x0001
#define CE_ALLABORT     0x0002
#define CE_ALLRETRY     0x0004
#define CE_ALLIGNORE    0x0008
#define CE_ALLACK       0x0010

/* FSH_CRITERROR answers */
#define CE_RETIGNORE    0x0000
#define CE_RETRETRY     0x0001
#define CE_RETFAIL      0x0003
#define CE_RETACK       0x0004

/* FSH_FLUSHBUF fDiscard */
#define FB_DISCNONE     0x0000
#define FB_DISCCLEAN    0x0001

/* FSH_GETOVERLAPBUF pre-read control */
#define GB_PRNOREAD     0x0001

/* FSH_QSYSINFO index */
#define QSI_SECSIZE     1
#define QSI_PROCID      2
#define QSI_THREADNO    3
#define QSI_VERIFY      4

/* FSH_REGISTERPERFCTRS fsFlags */
#define RPC_16BIT       0x0000
#define RPC_32BIT       0x0001

/* FSH_SEGALLOC flags */
#define SA_FLDT         0x0001      /* allocate from the LDT rather than GDT */
#define SA_FSWAP        0x0002      /* segment may be swapped                */
#define SA_FRINGMASK    0x6000
#define SA_FRING0       0x0000
#define SA_FRING1       0x2000
#define SA_FRING2       0x4000
#define SA_FRING3       0x6000

/* ------------------------------------------------------------------ */
/* Helper prototypes                                                    */
/* ------------------------------------------------------------------ */

/* --- semaphores --- */
int far pascal FSH_SEMCLEAR(unsigned long far *pSem);
int far pascal FSH_SEMREQUEST(unsigned long far *pSem, unsigned long cmsTimeout);
int far pascal FSH_SEMSET(unsigned long far *pSem);
int far pascal FSH_SEMSETWAIT(unsigned long far *pSem, unsigned long cmsTimeout);
int far pascal FSH_SEMWAIT(unsigned long far *pSem, unsigned long cmsTimeout);

/* --- buffers, memory --- */
int far pascal FSH_PROBEBUF(unsigned short operation, void far *pData, unsigned short cbData);
int far pascal FSH_SEGALLOC(unsigned short flags, unsigned long cbSeg, unsigned short far *pSel);
int far pascal FSH_SEGFREE(unsigned short sel);
int far pascal FSH_SEGREALLOC(unsigned short sel, unsigned long cbSeg);
int far pascal FSH_FORCENOSWAP(unsigned short sel);
int far pascal FSH_GETOVERLAPBUF(unsigned short hVPB, unsigned long iSec, unsigned long iSec2,
                                 unsigned long far *pisecBuf, char far * far *ppBuf);

/* --- volumes and device I/O --- */
int far pascal FSH_GETVOLPARM(unsigned short hVPB, struct vpfsi far * far *ppVPBfsi,
                              struct vpfsd far * far *ppVPBfsd);
int far pascal FSH_FINDDUPHVPB(unsigned short hVPB, unsigned short far *pHVPB);
int far pascal FSH_SETVOLUME(unsigned short hVPB, unsigned long fControl);
int far pascal FSH_DOVOLIO(unsigned short operation, unsigned short fAllowed,
                           unsigned short hVPB, char far *pData,
                           unsigned short far *pcSec, unsigned long iSec);
int far pascal FSH_DOVOLIO2(unsigned long hDev, unsigned short sfn, unsigned short cat,
                            unsigned short func, char far *pParm, unsigned short cbParm,
                            char far *pData, unsigned short cbData);
int far pascal FSH_DEVIOCTL(unsigned short FSDRaisedFlag, unsigned long hDev, unsigned short sfn,
                            unsigned short cat, unsigned short func,
                            char far *pParm, unsigned short cbParm,
                            char far *pData, unsigned short cbData);
int far pascal FSH_FLUSHBUF(unsigned short hVPB, unsigned short fDiscard);

/* --- names and characters --- */
int  far pascal FSH_CANONICALIZE(char far *pPathName, unsigned short cbPathBuf,
                                 char far *pPathBuf, char far *pFlags);
int  far pascal FSH_CHECKEANAME(unsigned short level, unsigned long len, char far *pEAName);
int  far pascal FSH_FINDCHAR(unsigned short nChars, char far *pChars, char far * far *ppStr);
int  far pascal FSH_ISCURDIRPREFIX(char far *pName);
void far pascal FSH_LOADCHAR(char far * far *ppStr, unsigned short far *pChar);
void far pascal FSH_PREVCHAR(char far *pBeg, char far * far *ppStr);
int  far pascal FSH_STORECHAR(unsigned short chDBCS, char far * far *ppStr);
int  far pascal FSH_UPPERCASE(char far *pName, unsigned short cbPathBuf, char far *pPathBuf);
int  far pascal FSH_WILDMATCH(char far *pPat, char far *pStr);
int  far pascal FSH_NAMEFROMSFN(unsigned short sfn, char far *pName, unsigned short far *pcbName);

/* --- sharing --- */
int far pascal FSH_ADDSHARE(char far *pName, unsigned short mode, unsigned short hVPB,
                            unsigned long far *phShare);
int far pascal FSH_REMOVESHARE(unsigned long hShare);

/* --- scheduling and diagnostics --- */
int far pascal FSH_CRITERROR(int cbMessage, char far *pMessage, int nSubs, char far *pSubs,
                             unsigned short fAllowed);
int far pascal FSH_INTERR(char far *pMsg, unsigned short cbMsg);
int far pascal FSH_QSYSINFO(unsigned short index, char far *pData, unsigned short cbData);
int far pascal FSH_REGISTERPERFCTRS(char far *pDataBlk, char far *pTextBlk, unsigned short fsFlags);
int far pascal FSH_YIELD(void);
int far pascal FSH_IOBOOST(void);
int far pascal FSH_GETPRIORITY(void);

/* --- mini file system variants --- */
int far pascal MFSH_DOVOLIO(char far *Data, unsigned short far *cSec, unsigned long iSec);
int far pascal MFSH_INTERR(char far *Msg, unsigned short cbMsg);
int far pascal MFSH_SEGALLOC(unsigned short Flag, unsigned long cbSeg, unsigned short far *Sel);
int far pascal MFSH_SEGFREE(unsigned short Sel);
int far pascal MFSH_SEGREALLOC(unsigned short Sel, unsigned long cbSeg);
int far pascal MFSH_SETBOOTDRIVE(unsigned short drive);
int far pascal MFSH_CALLRM(unsigned long far *Proc);
int far pascal MFSH_LOCK(unsigned short Sel, unsigned long far *Handle);
int far pascal MFSH_PHYSTOVIRT(unsigned long Addr, unsigned short Len, unsigned short far *Sel);
int far pascal MFSH_UNLOCK(unsigned long Handle);
int far pascal MFSH_UNPHYSTOVIRT(unsigned short Sel);
int far pascal MFSH_VIRT2PHYS(unsigned long VirtAddr, unsigned long far *PhysAddr);

#endif /* HFS2_RING0_FSH_H_INCLUDED */
