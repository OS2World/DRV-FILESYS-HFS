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

/* globals.c
   Internal data for the filter.
   */

/* Use MiniDDK headers instead of os2.h to avoid duplicate definitions */
/* FAR32 is a 32-bit far pointer type used in MiniDDK headers */
/* Only defined for M_I386 (32-bit), so define it for 16-bit compilation */
#ifndef FAR32
#define FAR32 _far
#endif

#define INCL_TYPES
#include <basedef.h>

/* APIENTRY calling convention for 16-bit OS/2 drivers */
#ifndef APIENTRY
#define APIENTRY _far _pascal
#endif

#define INCL_INITRP_ONLY
#include <reqpkt.h>

#include <dhcalls.h>
#include <iorb.h>

#include "hfsflt.h"
#include "globals.h"

/* Static data */

PFN       Device_Help               = 0;     /* DevHelp entry point */
ULONG     ppDataSeg                 = 0;     /* Phys. addr of data segment */
USHORT    InitComplete              = 0;     /* Driver initialized */
USHORT    ADDHandle                 = 0;     /* Handle of the filter */
USHORT    UnitEntryCount            = 0;     /* Number of units */
UNITENTRY UnitEntryTable[MAX_UNITS] = {0};   /* Unit info */
PIORB     IORBQueueHead             = NULL;  /* Head of IORB queue */
PIORB     IORBQueueTail             = NULL;  /* Tail of IORB queue */
USHORT    QueueLocked               = 0;     /* Set if IORB queue is being
						serviced */
USHORT    QueueBusy                 = 0;     /* Set if an IORB in the queue
						is being processed */
UCHAR     SectorBuffer[512]         = {0};   /* Buffer for sector transfers */
UCHAR     IORBBuf[MAX_IORB_SIZE]    = {0};   /* IORB */

/* --- END OF RESIDENT DATA --- */
/* Anything after this address will be released after initialization */
USHORT EndData = 0;

/* Initialization data */

BYTE    DeviceTable[MAX_DEVICETABLE_SIZE] = {0};

#define MSG_REPLACEMENT_STRING  1178

MSGTABLE MsgTbl = { MSG_REPLACEMENT_STRING,
		    1,
		    NULL };
