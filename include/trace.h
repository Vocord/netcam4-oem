/*\
 *  
 *  Copyright (C) 2004-2014 VOCORD, Inc. <info@vocord.com>
 *
 * This file is part of the P3SS API/ABI/VERIFICATION system.
 *
 * The P3SS API/ABI/VERIFICATION system is free software; you can
 * redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The part of the P3SS API/ABI/VERIFICATION system
 * is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with the part of the P3SS API/ABI/VERIFICATION system;
 * if not, see <http://www.gnu.org/licenses/>.
 *
 *
 *       Component for conform portion of the system.
\*/

/*\
 *  Description: trace OS specific functions header.
 *
 *       Author: alexr
 *      License: LGPL
 *
 *  tab-size=8
 *
\*/
#ifndef __TRACE_H__

#ifndef TRACE_FUNC
#define TRACE_FUNC(L, X, R, SS) { \
	R = X;	\
	TRACE(L, #X "" SS, R); \
}
#endif

#ifndef __GNUC__
#include <compilers/visualc/trace.h>
#else
#include <compilers/gnuc/trace.h>
#endif

#ifndef TRACE_BANNER
#define	TRACE_BANNER()		TRACEP(0, "%s:%d =====================> BANNER <===================\n", __func__ ,__LINE__);
#endif

#ifndef TRACE_LINEL
#define	TRACE_LINEL(L)		TRACEP(L, "%s:%d\n", __func__ ,__LINE__);
#endif

#ifndef TRACE_LINE
#define	TRACE_LINE()		TRACEP(0, "%s:%d\n", __func__ ,__LINE__);
#endif

#ifndef TRACENF_LINE
#define	TRACENF_LINE()		TRACEPNF(0, "%s:%d\n", __func__ ,__LINE__);
#endif

#ifndef TRACENF_LINEL
#define	TRACENF_LINEL(L)	TRACEPNF(L, "%s:%d\n", __func__ ,__LINE__);
#endif

#define __TRACE_H__
#endif
