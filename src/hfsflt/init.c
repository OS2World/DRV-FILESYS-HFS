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

/* init.c
   Driver initialization.
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

#include <devcmd.h>
#include <devclass.h>
#include <dskinit.h>
#include <bseerr.h>

#define INCL_INITRP_ONLY
#include <reqpkt.h>

#include <iorb.h>

#include <dhcalls.h>

#include "hfsflt.h"
#include "globals.h"
#include "initutil.h"
#include "banner.h"
#ifdef DEBUG
#include "log.h"
#endif

/* Boot message options (BASEDEV=HFSFLT.FLT [/V] [/Q]).  /W and /AA are accepted but
   have no effect: a BASEDEV driver runs before DosRead/DosBeep are available. */
static USHORT fQuiet, fVerbose;

static VOID Msg(char *m)
{
  if(!fQuiet) PrintMessage(m);
}

static VOID ParseFltArgs(char FAR *p)
{
  if(!p) return;
  while(*p && *p != ' ' && *p != '/') p++;      /* skip the driver name */
  while(*p) {
    if(*p == '/') {
      p++;
      if(*p == 'Q' || *p == 'q') fQuiet = 1;
      else if(*p == 'V' || *p == 'v') fVerbose = 1;
    }
    if(*p) p++;
  }
}

VOID Init(PRPINITIN pRPI)
{
  PRPINITOUT pRPO = (PRPINITOUT) pRPI;
  USHORT i, NumADDs, AddIndx;
  struct DevClassTableEntry FAR *pDCE;
  struct DevClassTableStruc FAR *pDCT;
  PCHAR pDataSeg = (PCHAR)&ppDataSeg;

  InitComplete = 1;
  Device_Help = pRPI->DevHlpEP;
  ParseFltArgs((char FAR *)pRPI->InitArgs);

  /* Get physical address of data segment */
  pDataSeg = (PCHAR)MK_FP(FP_SEG(&ppDataSeg), 0);
  DevHelp_VirtToPhys(pDataSeg, &ppDataSeg);

#ifdef DEBUG
  /* Initialize logging */
  init_log();
#endif

  /* Get address of ADD Table */
  if(DevHelp_GetDOSVar(DHGETDOSV_DEVICECLASSTABLE, DEVICECLASS_ADDDM, 
		       (PPVOID) &pDCT)) {
    pRPO->Unit    = 0;
    pRPO->CodeEnd = 0;
    pRPO->DataEnd = 0;
    pRPO->rph.Status = 0x810c;
    return;
  }

  NumADDs = pDCT->DCCount;

  /* For each ADD */
  for (pDCE=pDCT->DCTableEntries, AddIndx=0;
       AddIndx < NumADDs;
       AddIndx++, pDCE++)
  {
    USHORT j;
    ADDEntryPoint pADDBaseEP;
    PDEVICETABLE pDT;

  /* Get device table for the ADD */
    pADDBaseEP = (PVOID) MAKEP(pDCE->DCSelector, pDCE->DCOffset);

    if(GetDeviceTable(pADDBaseEP, (PDEVICETABLE) DeviceTable, 
		      sizeof(DeviceTable)))
      continue;

  /* For each adapter in the device table */

    pDT = (PDEVICETABLE) DeviceTable;
    for (j=0; j < pDT->TotalAdapters; j++ )
    {
      int k;
      NPADAPTERINFO npAI;
      NPUNITINFO npUI;
      USHORT NumUnits;

      npAI = pDT->pAdapter[j];
      NumUnits = npAI->AdapterUnits;

      /* Access UNITINFO for device */

      for ( npUI=npAI->UnitInfo, k=0; k < NumUnits; k++, npUI++ )
      {
	USHORT UnitFlags, UnitType;
	ADDEntryPoint pUnitADDEntry;

	UnitFlags = npUI->UnitFlags;
	UnitType  = npUI->UnitType;

	/* Determine where to send subsequent requests
	   (filter or ADD). */
	if ( npUI->FilterADDHandle )
	{
	  struct DevClassTableEntry FAR *pDCEFilter;
	  pDCEFilter = &pDCT->DCTableEntries[npUI->FilterADDHandle-1];
	  pUnitADDEntry = (PVOID) MAKEP(pDCEFilter->DCSelector,
					pDCEFilter->DCOffset    );
	}
	else
	  pUnitADDEntry  = pADDBaseEP;

	/* Check for removable disks */
	if( UnitType == UIB_TYPE_DISK &&
	    (UnitFlags & UF_REMOVABLE) )
	{
	  GEOMETRY geo;

	  if(GetGeometry(pUnitADDEntry, npUI->UnitHandle, &geo))
	    continue;

	  /* If at most 5760 sectors (2.88M), we grab it */
	  if(geo.TotalSectors <= 5760) {
	    PUNITENTRY pUnitEntry;

	    if(AllocateUnit(pUnitADDEntry, npUI->UnitHandle))
	      continue;

	    /* Save original unit handle and ADD entry point */
	    pUnitEntry = &UnitEntryTable[UnitEntryCount];
	    pUnitEntry->UnitHandle = npUI->UnitHandle;
	    pUnitEntry->pADDEntry = pUnitADDEntry;
	    pUnitEntry->Allocated = 0;
	    /* Copy UNITINFO */
	    ASM_CLD
	    pUnitEntry->newUnitInfo = *npUI;
	    
	    UnitEntryCount++;
	  }
	}
      }
    }
  }
  if(!UnitEntryCount) {
    /* No units found */
    Msg("HFS: HFS/2 could not be loaded. No removable disk units found.\r\n");
    pRPO->Unit = 0;
    pRPO->CodeEnd = 0;
    pRPO->DataEnd = 0;
    pRPO->rph.Status = STDON+STERR+ERROR_I24_QUIET_INIT_FAIL;
    return;
  }

  DevHelp_RegisterDeviceClass(ADDNAME,
			      (PFN)FilterIORBEntry,
			      ADDFLAGS,
			      DEVICECLASS_ADDDM,
			      &ADDHandle);
  if(!ADDHandle) {
    Msg("HFS: HFS/2 could not be loaded. Failed to register filter.\r\n");
    pRPO->Unit = 0;
    pRPO->CodeEnd = 0;
    pRPO->DataEnd = 0;
    pRPO->rph.Status = STDON+STERR+ERROR_I24_QUIET_INIT_FAIL;
    return;
  }    

  if(fVerbose)
    Msg("HFS: Filter " HFS_BANNER "\r\n");

  for(i=0; i < UnitEntryCount; i++) {
    /* Now modify UNITINFO */
     UnitEntryTable[i].newUnitInfo.UnitHandle = i;
     UnitEntryTable[i].newUnitInfo.FilterADDHandle = ADDHandle;
     ChangeUnitInfo(UnitEntryTable[i].pADDEntry,
		    UnitEntryTable[i].UnitHandle,
		    &UnitEntryTable[i].newUnitInfo);
     DeAllocateUnit(UnitEntryTable[i].pADDEntry, UnitEntryTable[i].UnitHandle);
#ifdef DEBUG
     printk("Got unit, handle = %hX.\r\n", UnitEntryTable[i].UnitHandle);
#endif
  }
  
  pRPO->Unit    = 0;
  pRPO->CodeEnd = (USHORT) Init;
  pRPO->DataEnd = (USHORT) &EndData;
  pRPO->Status  = STDON;
}
