#!/bin/sh

# very quick test -- observe output

case ${BASH_VERSION-} in *.*) set -o posix; shopt -s xpg_echo; esac
case ${ZSH_VERSION-} in *.*) emulate ksh; esac

set -euf

x () { printf '+ %s\n' "$*" >&2; "$@"; }

echo

case `exec ls -1rt ldpreload-ardet.c ldpreload-ardet.so 2>/dev/null`
in	(*ldpreload-ardet.c) x sh ldpreload-ardet.c
esac

rm -rf ttdir
mkdir ttdir
trap 'rm -rfv ttdir; echo' 0

strace -e trace=execve \
	ar qc ttdir/ar1.ar ldpreload-ardet.c ldpreload-ardet.so

LD_PRELOAD=$PWD/ldpreload-ardet.so strace -e trace=execve \
	ar qc ttdir/ar2.ar ldpreload-ardet.c ldpreload-ardet.so

strace -e trace=execve ranlib ttdir/ar1.ar
LD_PRELOAD=$PWD/ldpreload-ardet.so strace -e trace=execve ranlib ttdir/ar2.ar


for p in ar1 ar2; do hexdump -C ttdir/$p.ar > ttdir/$p.ar.hd; done
set +f
diff -U1 ttdir/ar?.*hd || :
set -f

# trap executed

# Local variables:
# mode: shell-script
# sh-basic-offset: 8
# tab-width: 8
# End:
# vi: set sw=8 ts=8
