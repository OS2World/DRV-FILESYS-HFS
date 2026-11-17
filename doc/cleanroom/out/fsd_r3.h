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

/* fsd_r3.h  (Ring 3; installed as src/Ring3/fsd.h)
 *
 * Declarations the control program needs in order to mirror the kernel's
 * file system driver interface in flat 32-bit user code.
 *
 * What is here:
 *   - the kernel-independent records (vpfsi, cdfsi, sffsi, fsfsi, filelock)
 *     whose layout MUST equal the Ring 0 definitions in fsd.h, because the
 *     two halves exchange copies of them through r0r3shar.h;
 *   - the value groups the Ring 3 sources use;
 *   - prototypes of the Ring 3 functions that stand in for the FS_xxx entry
 *     points (plain near pointers, no far/pascal).
 *
 * What is NOT here: the driver-private work areas (struct vpfsd, cdfsd,
 * sffsd, fsfsd).  Ring 3 gives those its own meaning in fsdep.h, which must
 * be included after this file.  Constants such as FILE_*, OPEN_*, FSIL_* and
 * FSCTL_FSDNAME come from the OS/2 headers (os2.h with the matching INCL_*).
 */

#ifndef HFS2_RING3_FSD_H_INCLUDED
#define HFS2_RING3_FSD_H_INCLUDED

#ifndef CCHMAXPATH
#define CCHMAXPATH 260
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

#pragma pack(push, 1)

struct hfs_quad {
    unsigned long lo;
    unsigned long hi;
};

#define VPBTEXTLEN 12

struct vpfsi {
    unsigned long  vpi_vid;
    unsigned long  vpi_hDEV;
    unsigned short vpi_bsize;
    unsigned long  vpi_totsec;
    unsigned short vpi_trksec;
    unsigned short vpi_nhead;
    char           vpi_text[VPBTEXTLEN];
    unsigned long  vpi_pDCS;        /* raw dword; not dereferenced in Ring 3 */
    unsigned long  vpi_pVCS;        /* raw dword; not dereferenced in Ring 3 */
    unsigned char  vpi_drive;
    unsigned char  vpi_unit;
    unsigned short vpi_flags;
};

struct cdfsi {
    unsigned short cdi_hVPB;
    unsigned short cdi_end;
    char           cdi_flags;
    char           cdi_curdir[CCHMAXPATH];
};

struct sffsi {
    unsigned long  sfi_mode;
    unsigned short sfi_hVPB;
    unsigned short sfi_ctime;
    unsigned short sfi_cdate;
    unsigned short sfi_atime;
    unsigned short sfi_adate;
    unsigned short sfi_mtime;
    unsigned short sfi_mdate;
    unsigned long  sfi_size;
    unsigned long  sfi_position;
    unsigned short sfi_UID;
    unsigned short sfi_PID;
    unsigned short sfi_PDB;
    unsigned short sfi_selfsfn;
    unsigned char  sfi_tstamp;
    unsigned short sfi_type;
    unsigned long  sfi_pPVDBFil;
    unsigned char  sfi_DOSattr;
    /* UNVERIFIED (partly): presence of the large-file members, see fsd.h and PROVENANCE.md U1 */
    struct hfs_quad sfi_sizel;
    struct hfs_quad sfi_positionl;
};

struct fsfsi {
    unsigned short fsi_hVPB;
};

/* UNVERIFIED: record layout, see fsd.h */
struct filelock {
    long lOffset;
    long lRange;
};

#pragma pack(pop)

/* ---- value groups used by the Ring 3 sources ------------------------ */

#define IOFL_WRITETHRU  0x10
#define IOFL_NOCACHE    0x20

#define FSA_ATTACH       0x00
#define FSA_DETACH       0x01
#define FSA_ATTACH_INFO  0x02

#define CD_EXPLICIT  0x00
#define CD_VERIFY    0x01
#define CD_FREE      0x02

#define CFP_RELBEGIN  0x00
#define CFP_RELCUR    0x01
#define CFP_RELEND    0x02

#define FA_RETRIEVE  0x00
#define FA_SET       0x01

#define FI_RETRIEVE  0x00
#define FI_SET       0x01

#define FF_NOPOS   0x00
#define FF_GETPOS  0x01

#define FLUSH_RETAIN   0x00
#define FLUSH_DISCARD  0x01

#define INFO_RETRIEVE  0x00
#define INFO_RETREIVE  0x00         /* historical misspelling, same value */
#define INFO_SET       0x01

#define MOUNT_MOUNT        0x00
#define MOUNT_VOL_REMOVED  0x01
#define MOUNT_RELEASE      0x02
#define MOUNT_ACCEPT       0x03

#define ST_SCREAT   0x01
#define ST_PCREAT   0x02
#define ST_SWRITE   0x04
#define ST_PWRITE   0x08
#define ST_SREAD    0x10
#define ST_PREAD    0x20

#define PI_RETRIEVE  0x00
#define PI_SET       0x01

#define FS_CL_ORDINARY  0
#define FS_CL_FORPROC   1
#define FS_CL_FORSYS    2

#define FS_COMMIT_ONE   1
#define FS_COMMIT_ALL   2

#define VUN_PASS1  0x0000
#define VUN_PASS2  0x0001

/* ---- Ring 3 stand-ins for the entry points -------------------------- */
/* Argument lists copied from the definitions in src/Ring3/*.c.  The extra
   leading vpfsd pointer, the trailing oError pointer and the vpfsi/vpfsd
   pair of FS_FSINFO are Ring 3 additions with no kernel counterpart.
   The vpfsd/cdfsd/sffsd/fsfsd tags are only ever used through pointers
   here, so they may stay incomplete until fsdep.h is included. */

struct vpfsd;
struct cdfsd;
struct sffsd;
struct fsfsd;

short int FSD_ATTACH(unsigned short flag, char *pDev, struct vpfsd *pvpfsd,
                     struct cdfsd *pcdfsd, void *pParm, unsigned short *pLen);

short int FS_CHDIR(struct vpfsd *pvpfsd, unsigned short flag, struct cdfsi *pcdfsi,
                   struct cdfsd *pcdfsd, char *pDir, unsigned short iCurDirEnd);
short int FS_MKDIR(struct vpfsd *pvpfsd, struct cdfsi *pcdfsi, struct cdfsd *pcdfsd,
                   char *pName, unsigned short iCurDirEnd, char *pEABuf,
                   unsigned short flags, unsigned long *oError);
short int FS_RMDIR(struct vpfsd *pvpfsd, struct cdfsi *pcdfsi, struct cdfsd *pcdfsd,
                   char *pName, unsigned short iCurDirEnd);

short int FS_FINDFIRST(struct vpfsd *pvpfsd, struct cdfsi *pcdfsi, struct cdfsd *pcdfsd,
                       char *pName, unsigned short iCurDirEnd, unsigned short attr,
                       struct fsfsi *pfsfsi, struct fsfsd *pfsfsd, char *pData,
                       unsigned short cbData, unsigned short *pcMatch,
                       unsigned short level, unsigned short flags, unsigned long *oError);
short int FS_FINDFROMNAME(struct vpfsd *pvpfsd, struct fsfsi *pfsfsi, struct fsfsd *pfsfsd,
                          char *pData, unsigned short cbData, unsigned short *pcMatch,
                          unsigned short level, unsigned long position, char *pName,
                          unsigned short flags, unsigned long *oError);
short int FS_FINDNEXT(struct vpfsd *pvpfsd, struct fsfsi *pfsfsi, struct fsfsd *pfsfsd,
                      char *pData, unsigned short cbData, unsigned short *pcMatch,
                      unsigned short level, unsigned short flag, unsigned long *oError);
short int FS_FINDCLOSE(struct vpfsd *pvpfsd, struct fsfsi *pfsfsi, struct fsfsd *pfsfsd);

short int FS_CHGFILEPTR(struct vpfsd *pvpfsd, struct sffsi *psffsi, struct sffsd *psffsd,
                        long offset, unsigned short type, unsigned short IOflag);
short int FS_CLOSE(struct vpfsd *pvpfsd, unsigned short type, unsigned short IOflag,
                   struct sffsi *psffsi, struct sffsd *psffsd);
short int FS_COMMIT(struct vpfsd *pvpfsd, unsigned short type, unsigned short IOflag,
                    struct sffsi *psffsi, struct sffsd *psffsd);
short int FS_COPY(struct vpfsd *pvpfsd, unsigned short flag, struct cdfsi *pcdfsi,
                  struct cdfsd *pcdfsd, char *pSrc, unsigned short iSrcCurDirEnd,
                  char *pDst, unsigned short iDstCurDirEnd, unsigned short nameType);
short int FS_DELETE(struct vpfsd *pvpfsd, struct cdfsi *pcdfsi, struct cdfsd *pcdfsd,
                    char *pFile, unsigned short iCurDirEnd);
short int FS_FILEATTRIBUTE(struct vpfsd *pvpfsd, unsigned short flag, struct cdfsi *pcdfsi,
                           struct cdfsd *pcdfsd, char *pName, unsigned short iCurDirEnd,
                           unsigned short *pAttr);
short int FS_FILEINFO(struct vpfsd *pvpfsd, unsigned short flag, struct sffsi *psffsi,
                      struct sffsd *psffsd, unsigned short level, char *pData,
                      unsigned short cbData, unsigned short IOflag, unsigned long *oError);
short int FS_MOVE(struct vpfsd *pvpfsd, struct cdfsi *pcdfsi, struct cdfsd *pcdfsd,
                  char *pSrc, unsigned short iSrcCurDirEnd, char *pDst,
                  unsigned short iDstCurDirEnd, unsigned short flags);
short int FS_NEWSIZE(struct vpfsd *pvpfsd, struct sffsi *psffsi, struct sffsd *psffsd,
                     unsigned long len, unsigned short IOflag);
short int FS_OPENCREATE(struct vpfsd *pvpfsd, struct cdfsi *pcdfsi, struct cdfsd *pcdfsd,
                        char *pName, unsigned short iCurDirEnd, struct sffsi *psffsi,
                        struct sffsd *psffsd, unsigned long openmode,
                        unsigned short openflag, unsigned short *pAction,
                        unsigned short attr, char *pEABuf, unsigned short *pfgenFlag,
                        unsigned long *oError);
short int FS_PATHINFO(struct vpfsd *pvpfsd, unsigned short flag, struct cdfsi *pcdfsi,
                      struct cdfsd *pcdfsd, char *pName, unsigned short iCurDirEnd,
                      unsigned short level, char *pData, unsigned short cbData,
                      unsigned long *oError);
short int FS_READ(struct vpfsd *pvpfsd, struct sffsi *psffsi, struct sffsd *psffsd,
                  char *pData, unsigned short *pLen, unsigned short IOflag);
short int FS_WRITE(struct vpfsd *pvpfsd, struct sffsi *psffsi, struct sffsd *psffsd,
                   char *pData, unsigned short *pLen, unsigned short IOflag);

short int FS_PROCESSNAME(char *pNameBuf);
short int FS_CANCELLOCKREQUEST(struct sffsi *psffsi, struct sffsd *psffsd,
                               struct filelock *pLockRange);
short int FS_FILELOCKS(struct sffsi *psffsi, struct sffsd *psffsd,
                       struct filelock *pUnLockRange, struct filelock *pLockRange,
                       unsigned long timeout, unsigned long flags);
short int FS_VERIFYUNCNAME(unsigned short flag, char *pName);

short int FS_MOUNT(unsigned short flag, struct vpfsi *pvpfsi, struct vpfsd *pvpfsd,
                   unsigned short hVPB, unsigned short vol_descr, unsigned char *pBoot);
short int FS_FSINFO(unsigned short flag, unsigned short hVPB, struct vpfsi *vpfsi,
                    struct vpfsd *vpfsd, char *pData, unsigned short cbData,
                    unsigned short level);
short int FS_FLUSHBUF(struct vpfsd *vpfsd, unsigned short flag);

#endif /* HFS2_RING3_FSD_H_INCLUDED */
