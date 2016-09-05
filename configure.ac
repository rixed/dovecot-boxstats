AC_INIT([dovecot-boxstats], [0.1], [rixed-github@happyleptic.org])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_INSTALL
AC_PROG_CC

AC_ARG_WITH(
  [dovecot],
  [AS_HELP_STRING([--with-dovecot],[prefix to dovecot installation])],
  [LDFLAGS="${LDFLAGS} -L${withval}/lib/dovecot";
   CPPFLAGS="${CPPFLAGS} -I${withval}/include"])

AC_CHECK_LIB(
  [dovecot],
  [fs_deinit],,
  AC_MSG_ERROR([missing dovecot library]))

#AC_CHECK_LIB(
#  [dovecot-storage],
#  [maildir_list_iter_init],,
#  AC_MSG_ERROR([missing dovecot-storage library]))

AC_CHECK_HEADER(
  [dovecot/lib.h],,
  AC_MSG_ERROR([missing dovecot/lib.h headers]),
  [#define HAVE_CONFIG_H])

AC_CHECK_HEADER(
  [dovecot/mail-storage.h],,
  AC_MSG_ERROR([missing dovecot/mail-storage.h headers]),
  [#define HAVE_CONFIG_H
#include <dovecot/lib.h>])

AC_PROG_CC_C99
if test "x$ac_cv_prog_cc_c99" = xno ; then
  AC_MSG_ERROR([What year is this?])
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
