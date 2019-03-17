#if 0 /* -*- mode: c; c-file-style: "stroustrup"; tab-width: 8; -*-
 set -euf
 case ${1-} in dbg) sfx=-dbg DEFS=-DDBG; shift ;; *) sfx= DEFS=-DDBG=0 ;; esac
 trg=${0##*''/}; trg=${trg%.c}$sfx.so; test ! -e "$trg" || rm "$trg"
 case ${1-} in '') set x -O2; shift; esac
 #case ${1-} in '') set x -ggdb; shift; esac
 x_exec () { printf %s\\n "$*" >&2; exec "$@"; }
 x_exec ${CC:-gcc} -std=c99 -shared -fPIC -o "$trg" "$0" $DEFS $@ -ldl
 exit $?
 */
#endif
/*
 * $ ldpreload-ardet.c $
 *
 * Author: Tomi Ollila -- too ät iki piste fi
 *
 *      Copyright (c) 2019 Tomi Ollila
 *          All rights reserved
 *
 * Created: Thu 14 Mar 2019 22:42:26 +0200 too
 * Last modified: Sun 17 Mar 2019 20:34:55 +0200 too
 */

// SPDX-License-Identifier: Artistic-2.0

#if 0 // change to '#if 1' whenever there is desire to see these...
#pragma GCC diagnostic warning "-Wpadded"
#pragma GCC diagnostic warning "-Wpedantic"
#endif

// gcc -dM -E -xc /dev/null | grep -i gnuc
// clang -dM -E -xc /dev/null | grep -i gnuc
#if defined (__GNUC__)

// to relax, change 'error' to 'warning' -- or even 'ignored'
// selectively. use #pragma GCC diagnostic push/pop to change
// the rules temporarily

#pragma GCC diagnostic error "-Wall"
#pragma GCC diagnostic error "-Wextra"

#if __GNUC__ >= 7

#pragma GCC diagnostic error "-Wimplicit-fallthrough"

#endif /* __GNUC__ >= 7 */

#pragma GCC diagnostic error "-Wstrict-prototypes"
#pragma GCC diagnostic error "-Winit-self"

// -Wformat=2 ¡currently! (2010-03) equivalent of the following 4
#pragma GCC diagnostic error "-Wformat"
#pragma GCC diagnostic error "-Wformat-nonliteral"
#pragma GCC diagnostic error "-Wformat-security"
#pragma GCC diagnostic error "-Wformat-y2k"

#pragma GCC diagnostic error "-Wcast-align"
#pragma GCC diagnostic error "-Wpointer-arith"
#pragma GCC diagnostic error "-Wwrite-strings"
#pragma GCC diagnostic error "-Wcast-qual"
#pragma GCC diagnostic error "-Wshadow"
#pragma GCC diagnostic error "-Wmissing-include-dirs"
#pragma GCC diagnostic error "-Wundef"
#pragma GCC diagnostic error "-Wbad-function-cast"
#ifndef __clang__
#pragma GCC diagnostic error "-Wlogical-op" // XXX ...
#endif
#pragma GCC diagnostic error "-Waggregate-return"
#pragma GCC diagnostic error "-Wold-style-definition"
#pragma GCC diagnostic error "-Wmissing-prototypes"
#pragma GCC diagnostic error "-Wmissing-declarations"
#pragma GCC diagnostic error "-Wredundant-decls"
#pragma GCC diagnostic error "-Wnested-externs"
#pragma GCC diagnostic error "-Winline"
#pragma GCC diagnostic error "-Wvla"
#pragma GCC diagnostic error "-Woverlength-strings"

//ragma GCC diagnostic error "-Wfloat-equal"
//ragma GCC diagnostic error "-Werror"
//ragma GCC diagnostic error "-Wconversion"

#endif /* defined (__GNUC__) */

#if defined(__linux__) && __linux__
#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define isizeof (int)sizeof
#define null ((void*)0)
#define WriteCS2(s) write(2, s, sizeof s - 1)

__attribute__((constructor))
static void ctor_fn(int argc, char ** argv, char ** envp)
{
    if (argc < 2) return;

    const int cl = strlen(argv[0]);
    //WriteCS2("ardet constructor: "); write(2, argv[0], cl); WriteCS2("\n");

    static char buf[16];

    if (cl >= 2 && strcmp(&argv[0][cl - 2], "ar") == 0) {
	if (cl == 2 || argv[0][cl - 3] == '/' || argv[0][cl - 3] == '-') {
	    // XXX some other (non-ar) command could end with -ar...
	    const int al = strlen(argv[1]);
	    if (al < isizeof buf - 2 && memchr(argv[1], 'D', al) == null) {
		buf[0] = 'D';
		memcpy(buf + 1, argv[1], cl + 1);
	    }
	    argv[1] = buf;
	}
    }
    else if (cl >= 6 && strcmp(&argv[0][cl - 6], "ranlib") == 0) {
	if (cl == 6 || argv[0][cl - 7] == '/' || argv[0][cl - 7] == '-') {
	    // XXX some other (non-ranlib) command could end with -ranlib...
	    if (strcmp(argv[1], "-D") != 0) {
		char * av[16];
		av[0] = argv[0];
		strcpy(buf, "-D"); av[1] = buf;
		for (int i = 2; i < isizeof av / isizeof av[0]; i++) {
		    av[i] = argv[i - 1];
		    if (av[i] == null) {
			execvpe(argv[0], av, envp);
			exit(1);
		    }
		}
	    }
	}
    }
}
