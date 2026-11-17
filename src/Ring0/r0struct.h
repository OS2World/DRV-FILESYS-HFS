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

/* r0struct.h
   Ring 0 structures
*/

#ifndef _R0STRUCT_H
#define _R0STRUCT_H

/* Missing types from DDK/IFS toolkit */
typedef unsigned long SEM;

/* Lock handle - matches r0comm.h definition */
typedef char HLOCK[12];

/* OPDATA - operation data structure (from IFS toolkit) */
#ifndef _R0R3SHAR_H
typedef struct opdata {
  unsigned long  OpData1;
  unsigned long  OpData2;
  unsigned long  OpData3;
  unsigned long  OpData4;
} OPDATA;

/* INITSTRUC - initialization structure passed to FS_INIT */
typedef struct initstruc {
  OPDATA *OpData;
  void   *Buf;
  unsigned long CPPID;
} INITSTRUC;
#endif

/* MK_FP - make far pointer from segment:offset (16:16 pointer) */
/* In 32-bit Watcom, use a simple cast to void* for flat memory model */
/* Undefine if already defined in Watcom headers */
#ifdef MK_FP
#undef MK_FP
#endif
#ifdef FP_SEG
#undef FP_SEG
#endif
#ifdef FP_OFF
#undef FP_OFF
#endif
#define MK_FP(seg, off) ((void *)((unsigned long)(seg) << 16 | (unsigned short)(off)))
#define FP_SEG(fp) ((unsigned short)((unsigned long)(fp) >> 16))
#define FP_OFF(fp) ((unsigned short)(unsigned long)(fp))

typedef struct cpdata {
  /* Ring 0 is messing with the buffers */
  SEM BufLock;

  /* A command is in the buffers */
  SEM CmdReady;
  
  /* The results of a command are in the buffers */
  SEM CmdComplete;
  
  /* Pointer to the shared operation buffer */
  OPDATA *OpData;
  
  /* Pointer to the shared data buffer */
  void *Buf;
} CPDATA;

/*
#define MAXCPRDYWAIT (10*1000)
#define MAXCPRESWAIT (10*1000)
*/

#define MAXCPRDYWAIT -1
#define MAXCPRESWAIT -1

#endif /* _R0STRUCT_H */
