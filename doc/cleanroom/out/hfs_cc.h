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
 * Written 2026 for the Open Watcom port (compiler environment only; contains
 * no kernel interface information).
 */

/* hfs_cc.h - calling-convention words for the Ring 0 sources.
 *
 * The Ring 0 makefile compiles 32-bit code with -zK, which turns the words
 * "far", "near" and "pascal" into ordinary identifiers.  The Ring 0 sources
 * still spell their entry points with those words, so give them a meaning
 * here: pointers are flat (far/near vanish) and pascal selects the system
 * calling convention of the 32-bit compiler.
 */

#ifndef HFS2_CC_H_INCLUDED
#define HFS2_CC_H_INCLUDED

#if defined(__WATCOMC__) && defined(__386__)

#ifdef far
#undef far
#endif
#ifdef near
#undef near
#endif
#ifdef pascal
#undef pascal
#endif

#define far
#define near
#define pascal _System

#endif /* __WATCOMC__ && __386__ */

#endif /* HFS2_CC_H_INCLUDED */
