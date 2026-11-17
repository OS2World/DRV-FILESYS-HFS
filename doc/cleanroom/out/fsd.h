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

/* fsd.h  (Ring 0)
 *
 * Kernel-facing declarations for the file system driver: the structures the
 * kernel hands to every FS_xxx entry point, the flag/constant groups of those
 * entry points, and the entry point prototypes themselves.
 *
 * The calling-convention words (far, pascal, _export, _loadds) are expected
 * to be provided by the including translation unit's environment, exactly as
 * the function definitions in stubs.c / r0comm.c already rely on.
 *
 * Constants for FILE_*, OPEN_*, FSIL_*, FSCTL_HANDLE/PATHNAME/FSDNAME and
 * CCHMAXPATH belong to the compiler's own OS/2 headers and are NOT repeated
 * here.
 */

#ifndef HFS2_RING0_FSD_H_INCLUDED
#define HFS2_RING0_FSD_H_INCLUDED

#include "hfs_cc.h"

#ifndef CCHMAXPATH
#define CCHMAXPATH 260              /* same value as the toolchain headers */
#endif

/* Error code returned when a volume cannot be (or is not) mounted.
 * UNVERIFIED: the real numeric value of ERROR_VOLUME_NOT_MOUNTED was not
 * found in any allowed source (not in Open Watcom's bseerr.h, not in the
 * osFree/JFS error lists). The value below is a PLACEHOLDER (it equals the
 * documented "not a DOS disk" code, 26) so that the sources compile; it must
 * be replaced by the true value once confirmed. See PROVENANCE.md, item U12.
 * The guard lets a compiler header that does define the name win. */
#ifndef ERROR_VOLUME_NOT_MOUNTED
#define ERROR_VOLUME_NOT_MOUNTED  26
#endif

/* ------------------------------------------------------------------ */
/* Value of the exported FS_ATTRIBUTE word                              */
/* ------------------------------------------------------------------ */

#define FSA_REMOTE      0x00000001UL    /* driver talks to a remote machine   */
#define FSA_UNC         0x00000002UL    /* driver understands UNC names       */
#define FSA_LOCK        0x00000004UL    /* driver wants byte-range lock calls */
#define FSA_LVL7        0x00000008UL    /* driver answers path-info level 7   */

/* ------------------------------------------------------------------ */
/* Sizes of the driver-private work areas the kernel reserves           */
/* ------------------------------------------------------------------ */

#define CDDWORKAREASIZE   8         /* per current-directory record */
#define SFDWORKAREASIZE  30         /* per open file                */
#define VPDWORKAREASIZE  36         /* per mounted volume           */
#define FSFSD_WORK_SIZE  24         /* per directory search         */

#pragma pack(push, 1)

/* A 64-bit quantity kept as two halves; the large-file fields of the open
   file record are never interpreted by HFS/2, only carried along. */
struct hfs_quad {
    unsigned long lo;
    unsigned long hi;
};

/* ------------------------------------------------------------------ */
/* Volume parameters                                                    */
/* ------------------------------------------------------------------ */

#define VPBTEXTLEN 12

struct vpfsi {                      /* kernel-owned part                    */
    unsigned long  vpi_vid;         /* volume serial number                 */
    unsigned long  vpi_hDEV;        /* device driver handle                 */
    unsigned short vpi_bsize;       /* bytes per sector                     */
    unsigned long  vpi_totsec;      /* sectors on the volume                */
    unsigned short vpi_trksec;      /* sectors per track                    */
    unsigned short vpi_nhead;       /* heads                                */
    char           vpi_text[VPBTEXTLEN]; /* volume label                    */
    unsigned long  vpi_pDCS;        /* 16:16 pointer, kept as raw dword     */
    unsigned long  vpi_pVCS;        /* 16:16 pointer, kept as raw dword     */
    unsigned char  vpi_drive;       /* drive letter index, A = 0            */
    unsigned char  vpi_unit;        /* unit within the device driver        */
    /* Present in the JFS-for-OS/2 record (S2) but absent from an older 16-bit
       OS/2 record (S5); HFS/2 never reads or writes it and the kernel owns the
       structure, so it only matters for sizeof(). Kernel versions that pass it
       are not documented in any allowed source (see PROVENANCE.md, U13). */
    unsigned short vpi_flags;       /* memory-capability flags (VPB_*)      */
};

#define VPB_ABOVE16M_ALLOWED   0x0001   /* device may use memory above 16 MB  */
#define VPB_NONCONTIG_ALLOWED  0x0002   /* FSD copes with discontiguous I/O   */

#define UNREAD_ID   0x534E4A52UL    /* volume id marking unreadable media */
#define DAMAGED_ID  0x00000000UL    /* volume id marking a damaged volume */

struct vpfsd {                      /* driver-private volume area */
    char vpd_work[VPDWORKAREASIZE];
};

/* ------------------------------------------------------------------ */
/* Current directory                                                    */
/* ------------------------------------------------------------------ */

struct cdfsi {
    unsigned short cdi_hVPB;        /* volume handle                        */
    unsigned short cdi_end;         /* length of the root part of the path  */
    char           cdi_flags;       /* CDI_* state bits                     */
    char           cdi_curdir[CCHMAXPATH]; /* current directory text        */
};

#define CDI_ISVALID    0x80
#define CDI_ISROOT     0x40
#define CDI_ISCURRENT  0x20

struct cdfsd {
    char cdd_work[CDDWORKAREASIZE];
};

/* ------------------------------------------------------------------ */
/* Open file (system file table entry)                                  */
/* ------------------------------------------------------------------ */

struct sffsi {
    unsigned long  sfi_mode;        /* access and sharing mode              */
    unsigned short sfi_hVPB;        /* volume handle                        */
    unsigned short sfi_ctime;       /* creation time                        */
    unsigned short sfi_cdate;       /* creation date                        */
    unsigned short sfi_atime;       /* last access time                     */
    unsigned short sfi_adate;       /* last access date                     */
    unsigned short sfi_mtime;       /* last write time                      */
    unsigned short sfi_mdate;       /* last write date                      */
    unsigned long  sfi_size;        /* length in bytes                      */
    unsigned long  sfi_position;    /* file pointer                         */
    unsigned short sfi_UID;         /* user id of first opener              */
    unsigned short sfi_PID;         /* process id of first opener           */
    unsigned short sfi_PDB;         /* DOS-box process block of first opener*/
    unsigned short sfi_selfsfn;     /* this entry's own file number         */
    unsigned char  sfi_tstamp;      /* ST_* timestamp request bits          */
    unsigned short sfi_type;        /* STYPE_* kind of object               */
    unsigned long  sfi_pPVDBFil;    /* performance counter block pointer    */
    unsigned char  sfi_DOSattr;     /* DOS attribute byte                   */
    /* UNVERIFIED (partly): sfi_DOSattr and the two 64-bit members below are
       confirmed by two sources (JFS S2, VirtualBox shared-folder FSD S4, which
       uses sfi_sizel / sfi_positionl / sfi_DOSattr) and an older 16-bit record
       (S5) stops before them, so they are a later kernel addition. Which kernel
       builds pass them to a 16-bit FSD is not documented; HFS/2 never touches
       them, so this affects sizeof() only. Halves are low word first (S3). */
    struct hfs_quad sfi_sizel;      /* 64-bit length                        */
    struct hfs_quad sfi_positionl;  /* 64-bit file pointer                  */
};

#define ST_SCREAT   0x01            /* stamp creation time          */
#define ST_PCREAT   0x02            /* propagate creation time      */
#define ST_SWRITE   0x04            /* stamp last-write time        */
#define ST_PWRITE   0x08            /* propagate last-write time    */
#define ST_SREAD    0x10            /* stamp last-access time       */
#define ST_PREAD    0x20            /* propagate last-access time   */

#define STYPE_FILE    0
#define STYPE_DEVICE  1
#define STYPE_NMPIPE  2
#define STYPE_FCB     4

struct sffsd {
    char sfd_work[SFDWORKAREASIZE];
};

/* ------------------------------------------------------------------ */
/* Directory search                                                     */
/* ------------------------------------------------------------------ */

struct fsfsi {
    unsigned short fsi_hVPB;
};

struct fsfsd {
    char fsd_work[FSFSD_WORK_SIZE];
};

/* Per-device record supplied while attaching (unused by HFS/2) */
struct devfsd {
    unsigned long FSDRef;
};

/* ------------------------------------------------------------------ */
/* Byte-range lock record (FS_FILELOCKS / FS_CANCELLOCKREQUEST)         */
/* ------------------------------------------------------------------ */

/* UNVERIFIED: layout taken from the user-level lock record of the OS/2
   DosProtectFileLocks call; the kernel-to-FSD record is believed to be
   identical but this has not been confirmed. */
struct filelock {
    long lOffset;                   /* start of range */
    long lRange;                    /* length of range */
};

/* ------------------------------------------------------------------ */
/* FS_FSCTL argument block                                              */
/* ------------------------------------------------------------------ */

struct SF {                         /* iArgType == FSCTL_ARG_FILEINSTANCE */
    struct sffsi far *psffsi;
    struct sffsd far *psffsd;
};

struct CD {                         /* iArgType == FSCTL_ARG_CURDIR */
    struct cdfsi far *pcdfsi;
    struct cdfsd far *pcdfsd;
    char far         *pPath;
    unsigned short    iCurDirEnd;
};

union argdat {                      /* iArgType == FSCTL_ARG_NULL: unused */
    struct SF sf;
    struct CD cd;
};

/* ------------------------------------------------------------------ */
/* FS_NMPIPE operation records                                          */
/* ------------------------------------------------------------------ */

struct phs_param  { short phs_len; short phs_dlen; short phs_pmode; };
struct npi_param  { short npi_len; short npi_dlen; short npi_level; };
struct npr_param  { short npr_len; short npr_dlen; short npr_nbyt; };
struct npw_param  { short npw_len; short npw_dlen; short npw_nbyt; };
struct npq_param  { short npq_len; short npq_dlen; long npq_timeo; short npq_prio; };
struct npx_param  {
    short          npx_len;
    unsigned short npx_ilen;
    char far      *npx_obuf;
    unsigned short npx_olen;
    unsigned short npx_nbyt;
    long           npx_timeo;
};
struct npp_param  {
    short          npp_len;
    unsigned short npp_dlen;
    unsigned short npp_nbyt;
    unsigned short npp_avl0;
    unsigned short npp_avl1;
    unsigned short npp_state;
};
struct npt_param  {
    short          npt_len;
    unsigned short npt_ilen;
    char far      *npt_obuf;
    unsigned short npt_olen;
    unsigned short npt_nbyt;
};
struct qnps_param {
    unsigned short qnps_len;
    unsigned short qnps_dlen;
    long           qnps_semh;
    unsigned short qnps_nbyt;
};
struct npc_param  { unsigned short npc_len; unsigned short npc_dlen; };
struct npd_param  { unsigned short npd_len; unsigned short npd_dlen; };

union npoper {
    struct phs_param  phs;
    struct npi_param  npi;
    struct npr_param  npr;
    struct npw_param  npw;
    struct npq_param  npq;
    struct npx_param  npx;
    struct npp_param  npp;
    struct npt_param  npt;
    struct qnps_param qnps;
    struct npc_param  npc;
    struct npd_param  npd;
};

/* ------------------------------------------------------------------ */
/* FS_DOPAGEIO command list                                             */
/* ------------------------------------------------------------------ */

struct PageCmd {
    unsigned char Cmd;              /* read / write / write-verify          */
    unsigned char Priority;
    unsigned char Status;           /* PGIO_ATTEMPTED / PGIO_FAILED         */
    unsigned char Error;            /* error code for this transfer         */
    unsigned long Addr;             /* physical or 16:16 virtual address    */
    unsigned long FileOffset;       /* page-aligned offset in the page file */
};

struct PageCmdHeader {
    unsigned char InFlags;          /* PGIO_FI_* */
    unsigned char OutFlags;         /* PGIO_FO_*, must be zero on entry */
    unsigned char OpCount;          /* number of entries that follow */
    unsigned char Pad;
    unsigned long Reserved1;
    unsigned long Reserved2;
    unsigned long Reserved3;
    struct PageCmd PageCmdList[1];
};

#pragma pack(pop)

#define PGIO_FI_ORDER   0x01
#define PGIO_FO_DONE    0x01
#define PGIO_FO_ERROR   0x02
#define PGIO_ATTEMPTED  0x0F
#define PGIO_FAILED     0xF0

/* flags for the *pFlags word of FS_OPENPAGEFILE */
#define PGIO_FIRSTOPEN  0x00000001UL
#define PGIO_PADDR      0x00004000UL
#define PGIO_VADDR      0x00008000UL

/* ------------------------------------------------------------------ */
/* Parameter value groups, one block per entry point                    */
/* ------------------------------------------------------------------ */

#define IOFL_WRITETHRU  0x10        /* IOflag: write through the cache */
#define IOFL_NOCACHE    0x20        /* IOflag: do not cache the data   */

#define FSA_ATTACH       0x00       /* FS_ATTACH flag values */
#define FSA_DETACH       0x01
#define FSA_ATTACH_INFO  0x02

#define CD_EXPLICIT  0x00           /* FS_CHDIR flag values */
#define CD_VERIFY    0x01
#define CD_FREE      0x02

#define CFP_RELBEGIN  0x00          /* FS_CHGFILEPTR origin values */
#define CFP_RELCUR    0x01
#define CFP_RELEND    0x02

#define FS_CL_ORDINARY  0           /* FS_CLOSE type values */
#define FS_CL_FORPROC   1
#define FS_CL_FORSYS    2

#define FS_COMMIT_ONE   1           /* FS_COMMIT type values */
#define FS_COMMIT_ALL   2

#define FA_RETRIEVE  0x00           /* FS_FILEATTRIBUTE flag values */
#define FA_SET       0x01

#define FI_RETRIEVE  0x00           /* FS_FILEINFO flag values */
#define FI_SET       0x01

#define FF_NOPOS  0x00              /* FS_FIND* position-return flag */
#define FF_GETPOS 0x01

#define FLUSH_RETAIN   0x00         /* FS_FLUSHBUF flag values */
#define FLUSH_DISCARD  0x01

#define FSCTL_ARG_FILEINSTANCE  0x01    /* FS_FSCTL iArgType values */
#define FSCTL_ARG_CURDIR        0x02
#define FSCTL_ARG_NULL          0x03

#define FSCTL_FUNC_NONE      0x00   /* FS_FSCTL function values */
#define FSCTL_FUNC_NEW_INFO  0x01
#define FSCTL_FUNC_EASIZE    0x02

#define INFO_RETRIEVE  0x00         /* FS_FSINFO flag values */
#define INFO_RETREIVE  0x00         /* historical misspelling, same value */
#define INFO_SET       0x01

#define MOUNT_MOUNT        0x00     /* FS_MOUNT flag values */
#define MOUNT_VOL_REMOVED  0x01
#define MOUNT_RELEASE      0x02
#define MOUNT_ACCEPT       0x03

#define NMP_GetPHandState   0x21    /* FS_NMPIPE OpType values */
#define NMP_SetPHandState   0x01
#define NMP_PipeQInfo       0x22
#define NMP_PeekPipe        0x23
#define NMP_ConnectPipe     0x24
#define NMP_DisconnectPipe  0x25
#define NMP_TransactPipe    0x26
#define NMP_READRAW         0x11
#define NMP_WRITERAW        0x31
#define NMP_WAITPIPE        0x53
#define NMP_CALLPIPE        0x54
#define NMP_QNmPipeSemState 0x58

#define FOC_NEEDEAS  0x01           /* FS_OPENCREATE *pfgenflag bit */

#define PI_RETRIEVE  0x00           /* FS_PATHINFO flag values */
#define PI_SET       0x01

#define SD_BEGIN     0x00           /* FS_SHUTDOWN type values */
#define SD_COMPLETE  0x01

#define VUN_PASS1  0x0000           /* FS_VERIFYUNCNAME flag values */
#define VUN_PASS2  0x0001

/* ------------------------------------------------------------------ */
/* Exported data                                                        */
/* ------------------------------------------------------------------ */

extern char pascal FS_NAME[];
extern unsigned long int pascal FS_ATTRIBUTE;

/* ------------------------------------------------------------------ */
/* Entry points (argument lists match stubs.c and r0comm.c)             */
/* ------------------------------------------------------------------ */

/* The OS/2 / DDK headers define FS_ATTACH as a plain number (the attach-flag
 * value), which collides with the entry point of the same name declared below.
 * Drop that macro here; the flag value is not needed by the Ring 0 sources. */
#ifdef FS_ATTACH
#undef FS_ATTACH
#endif

#define HFS_FSENTRY  short int far pascal _export _loadds

HFS_FSENTRY FS_ALLOCATEPAGESPACE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                 unsigned long lSize, unsigned long lWantContig);
HFS_FSENTRY FS_ATTACH(unsigned short flag, char far *pDev, struct vpfsd far *pvpfsd,
                      struct cdfsd far *pcdfsd, void far *pParm, unsigned short far *pLen);
HFS_FSENTRY FS_CANCELLOCKREQUEST(struct sffsi far *psffsi, struct sffsd far *psffsd,
                                 void far *pLockRange);
HFS_FSENTRY FS_CHDIR(unsigned short flag, struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                     char far *pDir, unsigned short iCurDirEnd);
HFS_FSENTRY FS_CHGFILEPTR(struct sffsi far *psffsi, struct sffsd far *psffsd,
                          long offset, unsigned short type, unsigned short IOflag);
HFS_FSENTRY FS_CLOSE(unsigned short type, unsigned short IOflag,
                     struct sffsi far *psffsi, struct sffsd far *psffsd);
HFS_FSENTRY FS_COMMIT(unsigned short type, unsigned short IOflag,
                      struct sffsi far *psffsi, struct sffsd far *psffsd);
HFS_FSENTRY FS_COPY(unsigned short flag, struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                    char far *pSrc, unsigned short iSrcCurDirEnd,
                    char far *pDst, unsigned short iDstCurDirEnd, unsigned short nameType);
HFS_FSENTRY FS_DELETE(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                      char far *pFile, unsigned short iCurDirEnd);
HFS_FSENTRY FS_DOPAGEIO(struct sffsi far *psffsi, struct sffsd far *psffsd,
                        struct PageCmdHeader far *pPageCmdList);
HFS_FSENTRY FS_FILEATTRIBUTE(unsigned short flag, struct cdfsi far *pcdfsi,
                             struct cdfsd far *pcdfsd, char far *pName,
                             unsigned short iCurDirEnd, unsigned short far *pAttr);
HFS_FSENTRY FS_FILEINFO(unsigned short flag, struct sffsi far *psffsi, struct sffsd far *psffsd,
                        unsigned short level, char far *pData, unsigned short cbData,
                        unsigned short IOflag);
HFS_FSENTRY FS_FILEIO(struct sffsi far *psffsi, struct sffsd far *psffsd,
                      char far *pCmdList, unsigned short cbCmdList,
                      unsigned short far *poError, unsigned short IOflag);
HFS_FSENTRY FS_FILELOCKS(struct sffsi far *psffsi, struct sffsd far *psffsd,
                         void far *pUnLockRange, void far *pLockRange,
                         unsigned long timeout, unsigned long flags);
HFS_FSENTRY FS_FINDCLOSE(struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd);
HFS_FSENTRY FS_FINDFIRST(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                         char far *pName, unsigned short iCurDirEnd, unsigned short attr,
                         struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd,
                         char far *pData, unsigned short cbData, unsigned short far *pcMatch,
                         unsigned short level, unsigned short flags);
HFS_FSENTRY FS_FINDFROMNAME(struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd,
                            char far *pData, unsigned short cbData,
                            unsigned short far *pcMatch, unsigned short level,
                            unsigned long position, char far *pName, unsigned short flags);
HFS_FSENTRY FS_FINDNEXT(struct fsfsi far *pfsfsi, struct fsfsd far *pfsfsd,
                        char far *pData, unsigned short cbData,
                        unsigned short far *pcMatch, unsigned short level,
                        unsigned short flags);
HFS_FSENTRY FS_FINDNOTIFYCLOSE(unsigned short handle);
HFS_FSENTRY FS_FINDNOTIFYFIRST(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                               char far *pName, unsigned short iCurDirEnd,
                               unsigned short attr, unsigned short far *pHandle,
                               char far *pData, unsigned short cbData,
                               unsigned short far *pcMatch, unsigned short level,
                               unsigned long timeout);
HFS_FSENTRY FS_FINDNOTIFYNEXT(unsigned short handle, char far *pData, unsigned short cbData,
                              unsigned short far *pcMatch, unsigned short level,
                              unsigned long timeout);
HFS_FSENTRY FS_FLUSHBUF(unsigned short hVPB, unsigned short flag);
HFS_FSENTRY FS_FSCTL(union argdat far *pArgdat, unsigned short iArgType, unsigned short func,
                     char far *pParm, unsigned short lenParm, unsigned short far *plenParmOut,
                     char far *pData, unsigned short lenData, unsigned short far *plenDataOut);
HFS_FSENTRY FS_FSINFO(unsigned short flag, unsigned short hVPB, char far *pData,
                      unsigned short cbData, unsigned short level);
HFS_FSENTRY FS_INIT(char far *szParm, unsigned long pDevHlp, unsigned long far *pMiniFSD);
HFS_FSENTRY FS_IOCTL(struct sffsi far *psffsi, struct sffsd far *psffsd,
                     unsigned short cat, unsigned short func,
                     char far *pParm, unsigned short lenParm, unsigned far *pParmLenInOut,
                     char far *pData, unsigned short lenData, unsigned far *pDataLenInOut);
HFS_FSENTRY FS_MKDIR(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd, char far *pName,
                     unsigned short iCurDirEnd, char far *pEABuf, unsigned short flags);
HFS_FSENTRY FS_MOUNT(unsigned short flag, struct vpfsi far *pvpfsi, struct vpfsd far *pvpfsd,
                     unsigned short hVPB, char far *pBoot);
HFS_FSENTRY FS_MOVE(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                    char far *pSrc, unsigned short iSrcCurDirEnd,
                    char far *pDst, unsigned short iDstCurDirEnd, unsigned short flags);
HFS_FSENTRY FS_NEWSIZE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                       unsigned long len, unsigned short IOflag);
HFS_FSENTRY FS_NMPIPE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                      unsigned short OpType, union npoper far *pOpRec,
                      char far *pData, char far *pName);
HFS_FSENTRY FS_OPENCREATE(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                          char far *pName, unsigned short iCurDirEnd,
                          struct sffsi far *psffsi, struct sffsd far *psffsd,
                          unsigned long ulOpenMode, unsigned short usOpenFlag,
                          unsigned short far *pusAction, unsigned short usAttr,
                          char far *pEABuf, unsigned short far *pfgenflag);
HFS_FSENTRY FS_OPENPAGEFILE(unsigned long far *pFlags, unsigned long far *pcMaxReq,
                            char far *pName, struct sffsi far *psffsi, struct sffsd far *psffsd,
                            unsigned short OpenMode, unsigned short OpenFlag,
                            unsigned short Attr, unsigned long Reserved);
HFS_FSENTRY FS_PATHINFO(unsigned short flag, struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                        char far *pName, unsigned short iCurDirEnd, unsigned short level,
                        char far *pData, unsigned short cbData);
HFS_FSENTRY FS_PROCESSNAME(char far *pNameBuf);
HFS_FSENTRY FS_READ(struct sffsi far *psffsi, struct sffsd far *psffsd,
                    char far *pData, unsigned short far *pLen, unsigned short IOflag);
HFS_FSENTRY FS_RMDIR(struct cdfsi far *pcdfsi, struct cdfsd far *pcdfsd,
                     char far *pName, unsigned short iCurDirEnd);
HFS_FSENTRY FS_SETSWAP(struct sffsi far *psffsi, struct sffsd far *psffsd);
HFS_FSENTRY FS_SHUTDOWN(unsigned short usType, unsigned long ulReserved);
HFS_FSENTRY FS_WRITE(struct sffsi far *psffsi, struct sffsd far *psffsd,
                     char far *pData, unsigned short far *pLen, unsigned short IOflag);

/* No definition exists on the Ring 0 side yet; argument list from the Ring 3
   counterpart in Ring3/misc.c. */
HFS_FSENTRY FS_VERIFYUNCNAME(unsigned short flag, char far *pName);

/* Returns nothing. */
void far pascal _export _loadds FS_EXIT(unsigned short uid, unsigned short pid,
                                        unsigned short pdb);

#endif /* HFS2_RING0_FSD_H_INCLUDED */
