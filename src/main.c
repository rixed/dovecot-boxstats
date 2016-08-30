#include <stdlib.h>

/* We must use dovecot config.h because it defined types that
 * we want to reuse. Problem is: it redefines some of the stuff
 * the autoconf would define for us as well. So we are going to
 * include it first (via lib.h, forcing HAVE_CONFIG_H) and then
 * will clear some of the variables we will have in common before
 * including our own. Oh my!*/
#undef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#include <dovecot/lib.h>

#undef PACKAGE
#undef VERSION
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_BUGREPORT
#undef PACKAGE_VERSION
#include "config.h"

#include <dovecot/mail-storage.h>

/* First objective is /make all/ producing a binary linked with
 * dovecot. */

int main(int nb_args, char **args)
{
  return EXIT_SUCCESS;
}
