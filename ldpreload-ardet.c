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
 * Last modified: Wed 22 Jan 2020 20:22:02 +0200 too
 */

// SPDX-License-Identifier: Artistic-2.0


// hint: gcc -dM -E -xc /dev/null | grep -i gnuc
// also: clang -dM -E -xc /dev/null | grep -i gnuc
#if defined (__GNUC__)

#if 0 // use of -Wpadded gets complicated, 32 vs 64 bit systems
#pragma GCC diagnostic warning "-Wpadded"
#endif

#if 1
#if __GNUC__ >= 5
#pragma GCC diagnostic warning "-Wsuggest-attribute=pure"
#pragma GCC diagnostic warning "-Wsuggest-attribute=const"
#pragma GCC diagnostic warning "-Wsuggest-attribute=noreturn"
#pragma GCC diagnostic warning "-Wsuggest-attribute=format"
#endif
#endif

#pragma GCC diagnostic error "-Wall"
#pragma GCC diagnostic error "-Wextra"

#if __GNUC__ >= 8 // impractically strict in gccs 5, 6 and 7
#pragma GCC diagnostic error "-Wpedantic"
#endif

#if __GNUC__ >= 7
// gcc manual says all kind of /* fall.*through */ regexp's work too
// but perhaps only when cpp does not filter comments out. thus...
#define FALL_THROUGH __attribute__ ((fallthrough))
#else
#define FALL_THROUGH ((void)0)
#endif

#ifndef __cplusplus
#pragma GCC diagnostic error "-Wstrict-prototypes"
#pragma GCC diagnostic error "-Wbad-function-cast"
#pragma GCC diagnostic error "-Wold-style-definition"
#pragma GCC diagnostic error "-Wmissing-prototypes"
#pragma GCC diagnostic error "-Wnested-externs"
#endif

// -Wformat=2 ¡currently! (2020-0202) equivalent of the following 4
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

#ifndef __clang__ // XXX revisit -- tried with clang 3.8.0
#pragma GCC diagnostic error "-Wlogical-op"
#endif

#ifndef __cplusplus // supported by c++ compiler but perhaps not worth having
#pragma GCC diagnostic error "-Waggregate-return"
#endif

#pragma GCC diagnostic error "-Wmissing-declarations"
#pragma GCC diagnostic error "-Wredundant-decls"
#pragma GCC diagnostic error "-Winline"
#pragma GCC diagnostic error "-Wvla"
#pragma GCC diagnostic error "-Woverlength-strings"
#pragma GCC diagnostic error "-Wuninitialized"

//ragma GCC diagnostic error "-Wfloat-equal"
//ragma GCC diagnostic error "-Werror"
//ragma GCC diagnostic error "-Wconversion"

// avoiding known problems (turning some settings set above to ignored)...
#if __GNUC__ == 4
#ifndef __clang__
#pragma GCC diagnostic ignored "-Winline" // gcc 4.4.6 ...
#pragma GCC diagnostic ignored "-Wuninitialized" // gcc 4.4.6, 4.8.5 ...
#endif
#endif

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
