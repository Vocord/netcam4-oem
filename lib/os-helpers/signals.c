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
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <trace.h>

#include <os-helpers/signals.h>

static si_code_t sc_SIGILL[] = {
  /* SIGILL */
  { ILL_ILLOPC,		"illegal opcode"},
  { ILL_ILLOPN,		"illegal operand"},
  { ILL_ILLADR,		"illegal addressing mode"},
  { ILL_ILLTRP,		"illegal trap"},
  { ILL_PRVOPC,		"privileged opcode"},
  { ILL_PRVREG,		"privileged register"},
  { ILL_COPROC,		"coprocessor error"},
  { ILL_BADSTK,		"internal stack error"},
  { 0,			NULL}
};

static si_code_t sc_SIGFPE[] = {
  /* SIGFPE */
  { FPE_INTDIV,		"integer divide by zero"},
  { FPE_INTOVF,		"integer overflow"},
  { FPE_FLTDIV,		"floating point divide by zero"},
  { FPE_FLTOVF,		"floating point overflow"},
  { FPE_FLTUND,		"floating point underflow"},
  { FPE_FLTRES,		"floating point inexact result"},
  { FPE_FLTINV,		"floating point invalid operation"},
  { FPE_FLTSUB,		"subscript out of range"},
  { 0,			NULL}
};

static si_code_t sc_SIGSEGV[] = {
  /* SIGSEGV */
  { SEGV_MAPERR, 	"address not mapped to object"},
  { SEGV_ACCERR, 	"invalid permissions for mapped object"},
  { 0,			NULL}
};

static si_code_t sc_SIGBUS[] = {
  /* SIGBUS */
  { BUS_ADRALN,		"invalid address alignment"},
  { BUS_ADRERR,		"non-existant physical address"},
  { BUS_OBJERR,		"object specific hardware error"},
  { 0,			NULL}
};

#ifndef __CYGWIN__
static si_code_t sc_SIGTRAP[] = {
  /* SIGTRAP */
  { TRAP_BRKPT,		"process breakpoint"},
  { TRAP_TRACE,		"process trace trap"},
  { 0,			NULL}
};
#endif
static si_code_t sc_SIGCHLD[] = {
  /* SIGCHLD */
  { CLD_EXITED,		"child has exited"},
  { CLD_KILLED,		"child was killed"},
  { CLD_DUMPED,		"child terminated abnormally"},
  { CLD_TRAPPED,	"traced child has trapped"},
  { CLD_STOPPED,	"child has stopped"},
  { CLD_CONTINUED,	"stopped child has continued"},
  { 0,			NULL}
};

#ifndef __CYGWIN__
static si_code_t sc_SIGPOLL[] = {
  /*  SIGPOLL */
  { POLL_IN,		"data input available"},
  { POLL_OUT,		"output buffers available"},
  { POLL_MSG,		"input message available"},
  { POLL_ERR,		"i/o error"},
  { POLL_PRI,		"high priority input available"},
  { POLL_HUP,		"device disconnected"},
  { 0,			NULL}
};
#endif
#if 0
static si_code_t sc_COMMON[] = {
  /* common */
  { SI_USER,		"kill, sigsend or raise"},
  { SI_KERNEL,		"The kernel"},
  { SI_QUEUE,		"sigqueue"},
  { SI_TIMER,		"timer expired"},
  { SI_MESGQ,		"mesq state changed"},
  { SI_ASYNCIO,		"AIO completed"},
  { SI_SIGIO,		"queued SIGIO"},
  { 0,			NULL}
};
#endif

static si_sigs_t sc_tbl[] = {
  { SIGILL, 		sc_SIGILL},
  { SIGFPE, 		sc_SIGFPE},
  { SIGSEGV, 		sc_SIGSEGV},
  { SIGBUS, 		sc_SIGBUS},
#ifndef __CYGWIN__
  { SIGTRAP, 		sc_SIGTRAP},
#endif
  { SIGCHLD, 		sc_SIGCHLD},
#ifndef __CYGWIN__
  { SIGPOLL, 		sc_SIGPOLL},
#endif
  { 0,			NULL}
};
#if 0
siginfo_t {
                  int      si_signo;  /* Signal number */
                  int      si_errno;  /* An errno value */
                  int      si_code;   /* Signal code */
                  pid_t    si_pid;    /* Sending process ID */
                  uid_t    si_uid;    /* Real user ID of sending process */
                  int      si_status; /* Exit value or signal */
                  clock_t  si_utime;  /* User time consumed */
                  clock_t  si_stime;  /* System time consumed */
                  sigval_t si_value;  /* Signal value */
                  int      si_int;    /* POSIX.1b signal */
                  void *   si_ptr;    /* POSIX.1b signal */
                  void *   si_addr;   /* Memory location which caused fault */
                  int      si_band;   /* Band event */
                  int      si_fd;     /* File descriptor */
              }
#endif

void show_si_attrs(int sig, siginfo_t *si, void *ptr, char *name)
{
	si_code_t *st = NULL;
	si_sigs_t *sc = &sc_tbl[0];

	TRACE(0, "\n%s sig=%2d (", name, sig);
	
	while(sc->sc) {
		if(si->si_signo == sc->val) {
			st = sc->sc;
			break;
		}
		sc++;
	}
	
	while(st && st->name) {
		if((si->si_code & 0xffff) == st->val) {
			TRACE(0, "%s)\n", st->name);
			break;
		}
		st++;
	}

	if(!st || !st->name) {
		TRACE(0, "unknown reason see si_code)\n");
	}
	
	TRACE(0, "       ptr:%08lx", (unsigned long)ptr);
	TRACE(0, "   si_code:%08x", si->si_code);
	TRACE(0, " killer pid:%d\n", si->si_pid);

	TRACE(0, "  si_signo:%8d",   si->si_signo);
	TRACE(0, "  si_ernno:%8d (%s)\n", si->si_errno, strerror(errno));

	switch(si->si_signo) {
	    case SIGILL:
	    case SIGFPE:
	    case SIGSEGV:
	    case SIGBUS:
		TRACE(0, "   si_addr:%08lx\n", (unsigned long)si->si_addr);
	    break;
#ifndef __CYGWIN__
	    case SIGPOLL:
		TRACE(0, "   si_band:%8ld  ", si->si_band);
		TRACE(0, "     si_fd:%8d\n", si->si_fd);
	    break;
#endif
	}

	TRACE_FLUSH();

	return;
}

void signal_handler(int sig, siginfo_t *si, void *ptr)
{
	show_si_attrs(sig, si, ptr, "");
	
	TRACE(0, "\n\n");
	TRACE_FLUSH();
/*	ADDSET(got_signals, sig);*/
}
