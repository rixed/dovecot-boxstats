#ifndef BOXSTATS_H_20160902
#define BOXSTATS_H_20160902

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

/*
#include <dovecot/mail-storage.h>
#include <dovecot/maildir-storage.h>
#include <dovecot/mailbox-list.h>
#include <dovecot/mailbox-list-private.h>
#include <dovecot/mailbox-list-iter.h>
#include <dovecot/mailbox-list-maildir.h>*/
// Files
#include <dovecot/fs-api.h>
#include <dovecot/array.h>
#include <dovecot/strfuncs.h>
#include <dovecot/data-stack.h>

/*
 * Scanning
 */

void scanner_class_register_all(void);

extern int scan_dir(struct fs *fs, char const *dir);

enum maildir_file_type {
  MAILDIR_FILE_TYPE_UNKNOWN_FILE, // Failed to identify, better not touch.
  MAILDIR_FILE_TYPE_UNKNOWN_DIR,  // Failed to identify, will recurse nonetheless.
  MAILDIR_FILE_TYPE_FOLDER,  // An IMAP folder (.Sent, .Archive, ...)
  MAILDIR_FILE_TYPE_MAILDIR, // Where the mails are stored (cur, new, tmp...)
  MAILDIR_FILE_TYPE_INDEX,   // Dovecot index
  MAILDIR_FILE_TYPE_CACHE,   // Dovecot cache
  MAILDIR_FILE_TYPE_LOG,     // Dovecot log
  MAILDIR_FILE_TYPE_LOG2,    // Dovecot previous log
  MAILDIR_FILE_TYPE_KEYWORDS,  // Keywords
  MAILDIR_FILE_TYPE_SUBSCRIPTION,  // Subscribed folders list
  /* Mapping from IMAP uid to last known name (and few other info).
   * Possible operation on those files: check what storage own how many files
   * on this folder (can be diverse). Also the proportion of msg which flags
   * changed or were expunged since their names were recorded in this file. */
  MAILDIR_FILE_TYPE_UIDLIST,
  /* A file with an email inside. Can check size, get stats on flags, and
   * size distribution. */
  MAILDIR_FILE_TYPE_MAIL,
};

char const *name_of_maildir_file_type(enum maildir_file_type);

int ident_file(struct fs_file *, enum maildir_file_type *);

#endif
