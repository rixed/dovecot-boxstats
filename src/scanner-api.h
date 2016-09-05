#ifndef SCANNER_API_H_20160905
#define SCANNER_API_H_20160905

#include "boxstats.h"
#include <stdbool.h>

/* A single scanner for the duration of the program, aggregating
 * statistics. */
struct scanner_vfuncs {
  struct scanner *(*alloc)(void);
  int (*init)(struct scanner *);
  void (*deinit)(struct scanner *);
  bool (*autodetect)(struct scanner *, struct stat const *, char const *basename);
  int (*scan)(struct scanner *, struct fs_file *, struct stat const *, char const *);
};

struct scanner {
  char const *name;
  struct scanner_vfuncs v;
};

#endif
