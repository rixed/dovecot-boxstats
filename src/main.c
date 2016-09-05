#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <libgen.h>

#include "boxstats.h"
#include "misc.h"

/* First objective is /make all/ producing a binary linked with
 * dovecot. */

static void syntax(char *arg0) {
  printf(
    "%s OPTIONS\n"
    "\n"
    "Options:\n"
    "  --help|-h    : This help. \n"
    "  --dir|-d DIR : Scan the mailbox in this directory.\n"
    "\n",
    basename(arg0));
}

int main(int nb_args, char **args)
{
  char const *dir = "./";
  char const *fs_driver = "posix";
  lib_init();

  while (1) {
    static struct option options[] = {
      { "dir", required_argument, NULL, 'd' },
      { "fs-driver", required_argument, NULL, 'D' },
      { "help", no_argument, NULL, 'h' },
      { NULL, 0, NULL, 0 },
    };
    int c = getopt_long(nb_args, args, "hd:D:", options, NULL);
    if (c == -1) break;
    switch (c) {
      case 'd':
        dir = optarg;
        break;
      case 'D':
        fs_driver = optarg;
        break;
      case 'h':
        syntax(args[0]);
        return EXIT_SUCCESS;
      case '?':
        syntax(args[0]);
        return EXIT_FAILURE;
    }
  }

  int res = EXIT_FAILURE; // prepare for the worse

  scanner_class_register_all();

  /* Low level file access using Dovecot FS class: */
  const struct fs_settings fs_set = {
    .username = NULL,
    .session_id = NULL,
    .base_dir = "/inexistant/base_dir",
    .temp_dir = "/tmp",
    .ssl_client_set = NULL,
    .root_path = "/inexistant/root_path",
    .temp_file_prefix = "Im_a_temp",
    .dns_client = NULL,
    .debug = 1,
    .enable_timing = 1,
  };
  struct fs *fs = NULL;
  char const *error = NULL;
  char const *fs_args = ""; // TODO
  if (0 != fs_init(fs_driver, fs_args, &fs_set, &fs, &error)) {
    fprintf(stderr, "Cannot init fs of type %s(args=%s): %s\n",
      fs_driver, fs_args, error);
    goto err1;
  }

  if (0 != scan_dir(fs, dir)) goto err2;

  res = EXIT_SUCCESS;
err2:
  fs_deinit(&fs);
err1:
  lib_deinit();
  return res;
}
