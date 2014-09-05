#ifndef __OS_HELPERS_SIGNALS_H__
#define __OS_HELPERS_SIGNALS_H__
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

#include <sys/types.h>
#include <signal.h>

#define SETSIG_ACT_INT(sa, sig, fun) { \
	sa.sa_sigaction = fun; \
	sa.sa_flags = SA_SIGINFO; \
	sigaction(sig, &sa, NULL); \
}

#define SETSIG_ACT(sa, sig, fun) { \
	sa.sa_sigaction = fun; \
	sa.sa_flags = SA_RESTART|SA_SIGINFO; \
	sigaction(sig, &sa, NULL); \
}

#define SETSIG_ACT_SAVE(sa, sig, fun, sa_old) { \
	sa.sa_sigaction = fun; \
	sa.sa_flags = SA_RESTART|SA_SIGINFO; \
	sigaction(sig, &sa, sa_old); \
}

#define SETSIG(sa, sig, fun) { \
	sa.sa_handler = fun; \
	sa.sa_flags = 0; \
	sigaction(sig, &sa, NULL); \
}

typedef struct {
	int	val;
	char	*name;
} si_code_t;

typedef struct {
	int		val;
	si_code_t	*sc;
} si_sigs_t;

#define __SI_KILL	(0 << 16)
#define __SI_TIMER	(1 << 16)
#define __SI_POLL	(2 << 16)
#define __SI_FAULT	(3 << 16)
#define __SI_CHLD	(4 << 16)
#define __SI_RT		(5 << 16)

extern void show_si_attrs(int sig, siginfo_t *si, void *ptr, char *name);
extern void signal_handler(int sig, siginfo_t *si, void *ptr);
#endif
