/* hfsflt.h
   Miscellaneous declarations.
   */

#ifndef _HFSFLT_H
#define _HFSFLT_H

/* APIENTRY calling convention for 16-bit OS/2 drivers */
#ifndef APIENTRY
#define APIENTRY _far _pascal
#endif

/* FAR32 is a 32-bit far pointer type (48-bit in 16-bit world) */
/* MiniDDK headers use FAR32 but it's only defined for M_I386 (32-bit) */
/* In 16-bit, we define it as regular FAR pointer */
#ifndef FAR32
#define FAR32 _far
#endif

#if defined(__WATCOMC__)
#include <i86.h>
#endif

#include <iorb.h>

#define ADDNAME                "HFS_FILTER"
#define ADDFLAGS               0

#define MAX_UNITS               28
#define MAX_DEVICETABLE_SIZE    (1*1024)
#define SECTOR_SIZE             512

/* Missing macros from DDK headers */
#ifndef OFFSETOF
#define OFFSETOF(s, f)    ((USHORT)&(((s *)0)->f))
#endif

#ifndef MAKEP
#define MAKEP(seg, off)   ((VOID _far *)((ULONG)(seg) << 16 | (USHORT)(off)))
#endif

/* 16-bit far pointer macros for Watcom */
#ifndef MK_FP
#define MK_FP(seg, off)   ((void _far *)((((unsigned long)(seg)) << 16) | (unsigned short)(off)))
#endif

#ifndef FP_SEG
#define FP_SEG(fp)        ((unsigned short)(((unsigned long)(fp)) >> 16))
#endif

#ifndef FP_OFF
#define FP_OFF(fp)        ((unsigned short)((unsigned long)(fp)))
#endif

/* Error codes */
#ifndef ERROR_I24_QUIET_INIT_FAIL
/* 21 (0x15). The old value 0x8000 | 0x0018 had no parentheses, so in
   STDON+STERR+ERROR_I24_QUIET_INIT_FAIL it overflowed the 16-bit status word
   and dropped the error bit. */
#define ERROR_I24_QUIET_INIT_FAIL   21
#endif

/* Device class constants (from DDK) */
#ifndef DEVICECLASS_ADDDM
#define DEVICECLASS_ADDDM   1   /* ADD (Adapter Device Driver) device class */
#endif

typedef VOID (FAR* ADDEntryPoint)(PIORB);
typedef PIORB (FAR* NotifyAddr)(PIORB);

typedef struct _UNITENTRY {
  ADDEntryPoint pADDEntry;        /* Original ADD entry point */
  USHORT UnitHandle;              /* Original unit handle */
  USHORT Allocated;               /* Set if unit is allocated */
  UNITINFO newUnitInfo;           /* New unit info */
} UNITENTRY, *PUNITENTRY;

/* From filter.c */
VOID FAR FilterIORBEntry(PIORB);
/*void FilterIORB(PIORB);*/
void CommandDone(PIORB pIORB);
void CallOriginalADD(PIORB pIORB);

/* From init.c */
VOID Init(PRPINITIN pRPI);

/* From io.c */
VOID FilterRead(PIORB pIORB);
USHORT read_sector(ULONG RBA, USHORT count, ULONG ppBuf,
		   ADDEntryPoint pADDEntry, USHORT UnitHandle);
USHORT SendIORB(PIORB pIORB, ADDEntryPoint ADDEntry);

/* Compiler-specific stuff */

#if defined(__MSC__)
#define ENABLE  _enable()
#define DISABLE _disable()
#elif defined(__BORLANDC__)
#define ENABLE	asm { sti }
#define DISABLE	asm { cli }
#elif defined(__WATCOMC__)
#define ENABLE	_enable()
#define DISABLE	_disable()
/* Watcom inline assembly for 16-bit */
#define ASM_CLD   _asm { cld }
#endif


#endif /* _HFSFLT_H */
