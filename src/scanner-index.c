#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "scanner-api.h"
#include "metric-api.h"
#include "metric-size.h"

struct scanner_index {
  struct scanner scanner;
  // list of stats: get the one from struct scanner
  struct metric_size file_size;  // TODO: inherited structs
  struct metric_size block_size;
};

extern struct scanner scanner_class_index;

static struct scanner *scanner_index_alloc(void)
{
  struct scanner_index *s = i_new(struct scanner_index, 1);
  s->scanner = scanner_class_index;
  return &s->scanner;
}

static int scanner_index_init(struct scanner *scanner)
{
  struct scanner_index *s = (struct scanner_index *)scanner; // DOWNCAST?
  metric_size_init(&s->file_size);
  metric_size_init(&s->block_size);
  return 0;
}

static void scanner_index_deinit(struct scanner *scanner)
{
  struct scanner_index *s = (struct scanner_index *)scanner; // DOWNCAST?
  s->file_size.metric.v.deinit(&s->file_size.metric);
  s->block_size.metric.v.deinit(&s->block_size.metric);
}

static bool scanner_index_autodetect(struct scanner *scanner, struct stat const *stat, char const *basename)
{
  (void)scanner;
  if (stat->st_mode & S_IFDIR) return 0;
  if (0 != strcmp("dovecot.index", basename)) return 0;
  return 1;
}

static int scanner_index_scan(struct scanner *scanner, struct fs_file *file, struct stat const *stat, char const *basename)
{
  (void)file;
  (void)basename;
  struct scanner_index *s = (struct scanner_index *)scanner; // DOWNCAST?

  size_t const file_size = stat->st_size;
  s->file_size.metric.v.add(&s->file_size.metric, file_size);
  size_t const block_size = 512 * stat->st_blocks;
  s->block_size.metric.v.add(&s->block_size.metric, block_size);

  return 0;
}

struct scanner scanner_class_index = {
  .name = "index file",
  .v = {
    .alloc = scanner_index_alloc,
    .init = scanner_index_init,
    .deinit = scanner_index_deinit,
    .autodetect = scanner_index_autodetect,
    .scan = scanner_index_scan,
  },
};
