#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "boxstats.h"
#include "scanner-api.h"
#include "misc.h"

extern struct scanner scanner_class_mail;
extern struct scanner scanner_class_index;
extern struct scanner scanner_class_cache;

/* We will change this array of classes into an array of instances */
struct scanner *scanners[] = {
  &scanner_class_mail,
  &scanner_class_index,
  &scanner_class_cache,
};

static int scan_file_metadata(struct fs_file *file)
{
  ARRAY_TYPE(fs_metadata) const *metadatas;
  if (0 != fs_get_metadata(file, &metadatas)) {
    return -1;
  }
  bool has_metadata = 0;
  struct fs_metadata const *md;
  array_foreach(metadatas, md) {
    if (! has_metadata) {
      printf("Metadata:");
      has_metadata = 1;
    }
    printf(" %s: %s", md->key, md->value);
  }
  if (has_metadata) printf("\n");

  return 0;
}

static int scan_file(struct fs_file *file)
{
  struct fs *fs = fs_file_fs(file);
  char const *path = fs_file_path(file);

  // Does backend support metadatas?
  if (fs_get_properties(fs) & FS_PROPERTY_METADATA) {
    if (0 != scan_file_metadata(file)) {
      fprintf(stderr, "Error while scanning file '%s' metadatas: %s\n",
              path, fs_last_error(fs));
      // Keep going!
    }
  }

  ssize_t path_len = strlen(path);
  assert(path_len > 0);
  assert(path[path_len - 1] != '/');
  ssize_t base_start;
  for (base_start = path_len - 2 ;
       base_start >= 0 && path[base_start] != '/' ;
       base_start --) ;
  assert(base_start < path_len);
  if (path[base_start] == '/') base_start ++;

  struct stat stat;
  if (0 != fs_stat(file, &stat)) {
    fprintf(stderr, "Cannot stat file '%s': %s",
            path, strerror(errno));
    return -1;
  }

  bool done = false;
  FOR_EACH(scanners, scanner) {
    if (! scanner->v.autodetect(scanner, &stat, path + base_start)) continue;
    printf("Scanning file '%s' as %s\n", path, scanner->name);
    if (0 != scanner->v.scan(scanner, file, &stat, path + base_start)) continue;
    done = true;
    break;
  } END_FOR

  if (stat.st_mode & S_IFDIR) {
    return scan_dir(fs, path);
  }

  if (! done) fprintf(stderr, "Unknown file '%s'\n", path);

  return 0;
}

static int scan_dir_with_flags(
  struct fs *fs, char const *dir, enum fs_iter_flags flags)
{
  int ret = 1;

  struct fs_iter *iter =
    fs_iter_init(fs, dir, flags);
  assert(iter); // This never fails

  const char *fname;
  while (NULL != (fname = fs_iter_next(iter))) T_BEGIN {
      char const *path = t_strconcat(dir, "/", fname, NULL);
      struct fs_file *file =
        fs_file_init(fs, path, FS_OPEN_MODE_READONLY);
      assert(file);  // Never fails! but...
      switch (fs_exists(file)) {
        case 1:
          if (0 != scan_file(file)) {
            fprintf(stderr, "Error while scanning file '%s': %s\n",
                    fname, fs_last_error(fs));
            // keep going!
          }
          break;
        case 0:
          fprintf(stderr, "File '%s' disappeared!\n", fname);
          break;
        case -1:
          fprintf(stderr, "Cannot open file '%s': %s\n",
                  fname, fs_last_error(fs));
          break;
        default:
          assert(!"Invalid return value");
      }
      fs_file_deinit(&file);
  } T_END;

  if (0 != fs_iter_deinit(&iter)) {
    goto err0;
  }

  ret = 0;
err0:
  return ret;
}

int scan_dir(struct fs *fs, char const *dir)
{
  int ret = 1;

  printf("Scanning %s with driver %s...\n",
         dir, fs_get_driver(fs));

  // Does backend support objectids?
  bool has_objectids = fs_get_properties(fs) & FS_PROPERTY_OBJECTIDS;
  bool depth_first = 1;
  enum fs_iter_flags flags =
    (has_objectids ? FS_ITER_FLAG_OBJECTIDS:0) |
    (depth_first ? FS_ITER_FLAG_DIRS:0) |
    FS_ITER_FLAG_NOCACHE;

  int ret1 = scan_dir_with_flags(fs, dir, flags);
  int ret2 = scan_dir_with_flags(fs, dir, flags ^ FS_ITER_FLAG_DIRS);
  if (ret1 == 0 && ret2 == 0) ret = 0;

  return ret;
}

static void scanner_classes_deinit(void)
{
  for (unsigned c = 0; c < SIZEOF_ARRAY(scanners); c++) {
    scanners[c]->v.deinit(scanners[c]);
  }
}

void scanner_class_register_all(void)
{
  /* Dovecot register the "classes" first, which are instances of the
   * base type, and then use those to instantiate the actual abjects.
   * This brings nothing that a static allocator would do, and plays
   * badly with multiple inheritance (which parent to choose to alloc?).
   * Luckily, dovecot object model does not allow multiple inheritance.
   * In this case we really want a unique instance of the objects not
   * the class so we will proceed differently; Still an inefficient (boxed) and
   * over-engineered way to build a constant list of struct! */
  FOR_EACH_P(scanners, scanner) {
    struct scanner *s = (*scanner)->v.alloc();
    s->v.init(s);  // in what cases is this useful that alloc does not call init?!
    *scanner = s;
  } END_FOR_P
  lib_atexit(scanner_classes_deinit);
}

