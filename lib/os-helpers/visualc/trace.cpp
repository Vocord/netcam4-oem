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
#include <trace.h>

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void TRACE( int, const char* fmt, ...) 
{
    char buf[16384];

    va_list args;
    int len;
  
    va_start( args, fmt);
//    len = _vscprintf( format, args ) // _vscprintf doesn't count 
//    + 1; // terminating '\0'
//    buffer = malloc( len * sizeof(char) );
    vsprintf( buf, fmt, args);
//    printf( buf);
//    free( buffer );

//    sprintf( buf);
    OutputDebugStringA( buf);
}

#ifdef __cplusplus
};
#endif
