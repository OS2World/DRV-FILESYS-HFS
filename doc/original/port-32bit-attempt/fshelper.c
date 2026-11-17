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
 */

/*
 * fshelper.c - Implementation of FSH_* (File System Helper) functions
 * using OS/2 DevHelp services. This replaces the IFS Toolkit's fshelper.lib.
 */

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#include <os2.h>

#include <dhcalls.h>
#include <string.h>

#include "fsh.h"
#include "r0struct.h"

/* ERROR_NOT_FOUND if not defined */
#ifndef ERROR_NOT_FOUND
#define ERROR_NOT_FOUND  1168
#endif

/* Semaphore key counter - unique keys for each semaphore */
static ULONG semKeyCounter = 1;

/******************************************************************************
** FSH_SEMCLEAR - Clear a semaphore (set to signaled state)
******************************************************************************/
int far pascal
FSH_SEMCLEAR(
    SEM far *pSem
) {
    return DevHelp_SemClear(*pSem);
}

/******************************************************************************
** FSH_SEMREQUEST - Request a semaphore (wait for it)
******************************************************************************/
int far pascal
FSH_SEMREQUEST(
    SEM far *pSem,
    unsigned long cmsTimeout
) {
    return DevHelp_SemRequest(*pSem, cmsTimeout);
}

/******************************************************************************
** FSH_SEMSET - Set a semaphore (reset to non-signaled state)
******************************************************************************/
int far pascal
FSH_SEMSET(
    SEM far *pSem
) {
    /* SemSet is equivalent to getting a new semaphore handle with SEMUSEFLAG_IN_USE */
    return DevHelp_SemHandle(*pSem, SEMUSEFLAG_IN_USE, pSem);
}

/******************************************************************************
** FSH_SEMWAIT - Wait on a semaphore
******************************************************************************/
int far pascal
FSH_SEMWAIT(
    SEM far *pSem,
    unsigned long cmsTimeout
) {
    return DevHelp_SemRequest(*pSem, cmsTimeout);
}

/******************************************************************************
** FSH_SEMSETWAIT - Set and wait on a semaphore
******************************************************************************/
int far pascal
FSH_SEMSETWAIT(
    SEM far *pSem,
    unsigned long cmsTimeout
) {
    int rc;
    rc = DevHelp_SemHandle(*pSem, SEMUSEFLAG_IN_USE, pSem);
    if (rc != NO_ERROR) return rc;
    return DevHelp_SemRequest(*pSem, cmsTimeout);
}

/******************************************************************************
** FSH_PROBEBUF - Probe a buffer for read/write access
******************************************************************************/
int far pascal
FSH_PROBEBUF(
    unsigned short operation,
    void far *pData,
    unsigned short cbData
) {
    UCHAR accessFlag;
    
    if (operation == PB_OPWRITE) {
        accessFlag = VERIFY_READWRITE;
    } else {
        accessFlag = VERIFY_READONLY;
    }
    
    /* In flat 32-bit model, we need to verify the linear address */
    return DevHelp_VerifyAccess(FP_SEG(pData), cbData, FP_OFF(pData), accessFlag);
}

/******************************************************************************
** FSH_GETVOLPARM - Get volume parameters (VPFSI/VPFSD pointers)
******************************************************************************/
int far pascal
FSH_GETVOLPARM(
    unsigned short hVPB,
    struct vpfsi far * far *ppVPBfsi,
    struct vpfsd far * far *ppVPBfsd
) {
    /* This is a stub - the actual implementation would get VPB info from the kernel */
    /* For now, return ERROR_NOT_SUPPORTED */
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_DOVOLIO - Do volume I/O (read/write sectors)
******************************************************************************/
int far pascal
FSH_DOVOLIO(
    unsigned short operation,
    unsigned short fAllowed,
    unsigned short hVPB,
    char far *pData,
    unsigned short far *pcSec,
    unsigned long iSec
) {
    /* This is a stub - the actual implementation would use DevHelp to do I/O */
    /* For now, return ERROR_NOT_SUPPORTED */
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_FINDDUPHVPB - Find duplicate VPB handle
******************************************************************************/
int far pascal
FSH_FINDDUPHVPB(
    unsigned short hVPB,
    unsigned short far *pHVPB
) {
    /* This is a stub */
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_WILDMATCH - Wildcard matching
******************************************************************************/
int far pascal
FSH_WILDMATCH(
    char far *pPat,
    char far *pStr
) {
    /* Simple wildcard matching implementation */
    while (*pPat && *pStr) {
        if (*pPat == '*') {
            pPat++;
            if (!*pPat) return TRUE;  /* * at end matches everything */
            while (*pStr) {
                if (FSH_WILDMATCH(pPat, pStr)) return TRUE;
                pStr++;
            }
            return FALSE;
        } else if (*pPat == '?' || *pPat == *pStr) {
            pPat++;
            pStr++;
        } else {
            return FALSE;
        }
    }
    return (*pPat == *pStr) ? TRUE : FALSE;
}

/******************************************************************************
** FSH_UPPERCASE - Convert string to uppercase
******************************************************************************/
int far pascal
FSH_UPPERCASE(
    char far *pName,
    unsigned short cbPathBuf,
    char far *pPathBuf
) {
    unsigned short i;
    for (i = 0; i < cbPathBuf && pName[i]; i++) {
        char c = pName[i];
        if (c >= 'a' && c <= 'z') {
            pPathBuf[i] = c - 'a' + 'A';
        } else {
            pPathBuf[i] = c;
        }
    }
    if (i < cbPathBuf) pPathBuf[i] = '\0';
    return NO_ERROR;
}

/******************************************************************************
** FSH_SETVOLUME - Set volume state
******************************************************************************/
int far pascal
FSH_SETVOLUME(
    unsigned short hVPB,
    unsigned long fControl
) {
    /* This is a stub */
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_FLUSHBUF - Flush volume buffers
******************************************************************************/
int far pascal
FSH_FLUSHBUF(
    unsigned short hVPB,
    unsigned short fDiscard
) {
    /* This is a stub */
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_CANONICALIZE - Canonicalize a path name
******************************************************************************/
int far pascal
FSH_CANONICALIZE(
    char far *pPathName,
    unsigned short cbPathBuf,
    char far *pPathBuf,
    char far *pFlags
) {
    /* This is a stub - simple copy for now */
    strncpy(pPathBuf, pPathName, cbPathBuf - 1);
    pPathBuf[cbPathBuf - 1] = '\0';
    return NO_ERROR;
}

/******************************************************************************
** FSH_INTERR - Internal error handler
******************************************************************************/
int far pascal
FSH_INTERR(
    char far *pMsg,
    unsigned short cbMsg
) {
    return DevHelp_InternalError(pMsg, cbMsg);
}

/******************************************************************************
** FSH_YIELD - Yield CPU
******************************************************************************/
int far pascal
FSH_YIELD(void) {
    return DevHelp_Yield();
}

/******************************************************************************
** FSH_IOBOOST - Boost I/O priority
******************************************************************************/
int far pascal
FSH_IOBOOST(void) {
    /* This is a stub */
    return NO_ERROR;
}

/******************************************************************************
** FSH_GETPRIORITY - Get current thread priority
******************************************************************************/
int far pascal
FSH_GETPRIORITY(void) {
    /* This is a stub */
    return 0;
}

/******************************************************************************
** FSH_QSYSINFO - Query system information
******************************************************************************/
int far pascal
FSH_QSYSINFO(
    unsigned short index,
    char far *pData,
    unsigned short cbData
) {
    /* This is a stub */
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_ADDSHARE - Add a share
******************************************************************************/
int far pascal
FSH_ADDSHARE(
    char far *pName,
    unsigned short mode,
    unsigned short hVPB,
    unsigned long far *phShare
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_REMOVESHARE - Remove a share
******************************************************************************/
int far pascal
FSH_REMOVESHARE(
    unsigned long hShare
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_CHECKEANAME - Check EA name validity
******************************************************************************/
int far pascal
FSH_CHECKEANAME(
    unsigned short level,
    unsigned long len,
    char far *pEAName
) {
    return NO_ERROR;
}

/******************************************************************************
** FSH_CRITERROR - Critical error handler
******************************************************************************/
int far pascal
FSH_CRITERROR(
    int cbMessage,
    char far *pMessage,
    int nSubs,
    char far *pSubs,
    unsigned short fAllowed
) {
    return CE_RETFAIL;
}

/******************************************************************************
** FSH_DEVIOCTL - Device IOCTL
******************************************************************************/
int far pascal
FSH_DEVIOCTL(
    unsigned short FSDRaisedFlag,
    unsigned long hDev,
    unsigned short sfn,
    unsigned short cat,
    unsigned short func,
    char far *pParm,
    unsigned short cbParm,
    char far *pData,
    unsigned short cbData
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_DOVOLIO2 - Do volume I/O (extended)
******************************************************************************/
int far pascal
FSH_DOVOLIO2(
    unsigned long hDev,
    unsigned short sfn,
    unsigned short cat,
    unsigned short func,
    char far *pParm,
    unsigned short cbParm,
    char far *pData,
    unsigned short cbData
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_FINDCHAR - Find character in string
******************************************************************************/
int far pascal
FSH_FINDCHAR(
    unsigned short nChars,
    char far *pChars,
    char far * far *ppStr
) {
    unsigned short i;
    while (**ppStr) {
        for (i = 0; i < nChars; i++) {
            if (**ppStr == pChars[i]) return NO_ERROR;
        }
        (*ppStr)++;
    }
    return ERROR_NOT_FOUND;
}

/******************************************************************************
** FSH_GETOVERLAPBUF - Get overlap buffer
******************************************************************************/
int far pascal
FSH_GETOVERLAPBUF(
    unsigned short hVPB,
    unsigned long iSec,
    unsigned long iSec2,
    unsigned long far *pisecBuf,
    char far * far *ppBuf
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_FORCENOSWAP - Force memory to be non-swappable
******************************************************************************/
int far pascal
FSH_FORCENOSWAP(
    unsigned short sel
) {
    return DevHelp_Lock(sel, LOCKTYPE_LONG_ANYMEM, WAIT_IS_NOT_INTERRUPTABLE, NULL);
}

/******************************************************************************
** FSH_ISCURDIRPREFIX - Check if path is current directory prefix
******************************************************************************/
int far pascal
FSH_ISCURDIRPREFIX(
    char far *pName
) {
    return FALSE;
}

/******************************************************************************
** FSH_LOADCHAR - Load character from string (DBCS aware)
******************************************************************************/
void far pascal
FSH_LOADCHAR(
    char far * far *ppStr,
    unsigned short far *pChar
) {
    *pChar = (unsigned char)**ppStr;
    (*ppStr)++;
}

/******************************************************************************
** FSH_PREVCHAR - Get previous character (DBCS aware)
******************************************************************************/
void far pascal
FSH_PREVCHAR(
    char far *pBeg,
    char far * far *ppStr
) {
    if (*ppStr > pBeg) {
        (*ppStr)--;
    }
}

/******************************************************************************
** FSH_NAMEFROMSFN - Get name from system file number
******************************************************************************/
int far pascal
FSH_NAMEFROMSFN(
    unsigned short sfn,
    char far *pName,
    unsigned short far *pcbName
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_REGISTERPERFCTRS - Register performance counters
******************************************************************************/
int far pascal
FSH_REGISTERPERFCTRS(
    char far *pDataBlk,
    char far *pTextBlk,
    unsigned short fsFlags
) {
    return DevHelp_RegisterPerfCtrs(pDataBlk, pTextBlk, fsFlags);
}

/******************************************************************************
** FSH_SEGALLOC - Allocate segment
******************************************************************************/
int far pascal
FSH_SEGALLOC(
    unsigned short flags,
    unsigned long cbSeg,
    unsigned short far *pSel
) {
    return DevHelp_AllocGDTSelector(pSel, (USHORT)(cbSeg / 65536 + 1));
}

/******************************************************************************
** FSH_SEGFREE - Free segment
******************************************************************************/
int far pascal
FSH_SEGFREE(
    unsigned short sel
) {
    return DevHelp_FreeGDTSelector(sel);
}

/******************************************************************************
** FSH_SEGREALLOC - Reallocate segment
******************************************************************************/
int far pascal
FSH_SEGREALLOC(
    unsigned short sel,
    unsigned long cbSeg
) {
    return ERROR_NOT_SUPPORTED;
}

/******************************************************************************
** FSH_STORECHAR - Store character to string (DBCS aware)
******************************************************************************/
int far pascal
FSH_STORECHAR(
    unsigned short chDBCS,
    char far * far *ppStr
) {
    **ppStr = (char)chDBCS;
    (*ppStr)++;
    return NO_ERROR;
}

/* MFSH_* functions - Multi-threaded versions (stubs) */
int far pascal MFSH_DOVOLIO(char far *Data, unsigned short far *cSec, unsigned long iSec) { return ERROR_NOT_SUPPORTED; }
int far pascal MFSH_INTERR(char far *Msg, unsigned short cbMsg) { return DevHelp_InternalError(Msg, cbMsg); }
int far pascal MFSH_SEGALLOC(unsigned short Flag, unsigned long cbSeg, unsigned short far *Sel) { return FSH_SEGALLOC(Flag, cbSeg, Sel); }
int far pascal MFSH_SEGFREE(unsigned short Sel) { return FSH_SEGFREE(Sel); }
int far pascal MFSH_SEGREALLOC(unsigned short Sel, unsigned long cbSeg) { return ERROR_NOT_SUPPORTED; }
int far pascal MFSH_SETBOOTDRIVE(unsigned short drive) { return NO_ERROR; }
int far pascal MFSH_CALLRM(unsigned long far *Proc) { return ERROR_NOT_SUPPORTED; }
int far pascal MFSH_LOCK(unsigned short Sel, unsigned long far *Handle) { return DevHelp_Lock(Sel, LOCKTYPE_LONG_ANYMEM, WAIT_IS_NOT_INTERRUPTABLE, Handle); }
int far pascal MFSH_PHYSTOVIRT(unsigned long Addr, unsigned short Len, unsigned short far *Sel) { return ERROR_NOT_SUPPORTED; }
int far pascal MFSH_UNLOCK(unsigned long Handle) { return DevHelp_UnLock(Handle); }
int far pascal MFSH_UNPHYSTOVIRT(unsigned short Sel) { return ERROR_NOT_SUPPORTED; }
int far pascal MFSH_VIRT2PHYS(unsigned long VirtAddr, unsigned long far *PhysAddr) { return DevHelp_VirtToPhys((PVOID)VirtAddr, PhysAddr); }