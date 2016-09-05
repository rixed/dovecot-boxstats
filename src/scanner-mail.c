#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "metric-size.h"
#include "metric-bool.h"
#include "scanner-api.h"

struct scanner_mail {
  struct scanner scanner;
  // list of stats: get the one from struct scanner
  struct metric_size file_size;  // TODO: inherited structs
  struct metric_size block_size;
  struct metric_size head_file_size;
  struct metric_size head_virt_size;
  struct metric_bool file_size_differ;
  struct metric_bool crlf_stored;
};

extern struct scanner scanner_class_mail;

static struct scanner *scanner_mail_alloc(void)
{
  struct scanner_mail *s = i_new(struct scanner_mail, 1);
  s->scanner = scanner_class_mail;
  return &s->scanner;
}

static int scanner_mail_init(struct scanner *scanner)
{
  struct scanner_mail *s = (struct scanner_mail *)scanner; // DOWNCAST?
  metric_size_init(&s->file_size);
  metric_size_init(&s->block_size);
  metric_size_init(&s->head_file_size);
  metric_size_init(&s->head_virt_size);
  metric_bool_init(&s->file_size_differ);
  metric_bool_init(&s->crlf_stored);
  return 0;  // above inits cannot fail
}

static void scanner_mail_deinit(struct scanner *scanner)
{
  struct scanner_mail *s = (struct scanner_mail *)scanner; // DOWNCAST?
  s->file_size.metric.v.deinit(&s->file_size.metric);
  s->file_size.metric.v.deinit(&s->block_size.metric);
  s->file_size.metric.v.deinit(&s->head_file_size.metric);
  s->file_size.metric.v.deinit(&s->head_virt_size.metric);
  s->file_size.metric.v.deinit(&s->file_size_differ.metric);
  s->file_size.metric.v.deinit(&s->crlf_stored.metric);
}

static bool scanner_mail_autodetect(struct scanner *scanner, struct stat const *stat, char const *basename)
{
  (void)scanner;
  if (stat->st_mode & S_IFDIR) return 0;
  if (NULL == strstr(basename, ":2,")) return 0;
  return 1;
}

static int size_from_name(char const *delim, char const *name, size_t not_after, size_t *size)
{
  char const *found_delim = strstr(delim, name);
  if (! found_delim || found_delim >= name + not_after) return -1;

  char *end;
  *size = strtoull(found_delim + strlen(delim), &end, 10);

  // TODO: check end
  if (*end != ':' && *end != ',' && *end != '\0') {
    fprintf(stderr, "Possibly incorrect flag vsize in '%s'.\n", name);
    // The show must go on
  }
  return 0;
}

static int scanner_mail_scan(struct scanner *scanner, struct fs_file *file, struct stat const *stat, char const *basename)
{
  (void)file;
  struct scanner_mail *s = (struct scanner_mail *)scanner; // DOWNCAST?

  size_t const file_size = stat->st_size;
  s->file_size.metric.v.add(&s->file_size.metric, file_size);
  size_t const block_size = 512 * stat->st_blocks;
  s->block_size.metric.v.add(&s->block_size.metric, block_size);

  int end_stdname = (strstr(basename, ":2,") - basename);
  size_t sz;
  if (0 <= size_from_name(",S=", basename, end_stdname, &sz)) {
    s->head_file_size.metric.v.add(&s->head_file_size.metric, sz);
    s->file_size_differ.metric.v.add(&s->file_size_differ.metric, file_size != sz);
  }
  if (0 <= size_from_name(",W=", basename, end_stdname, &sz)) {
    s->head_virt_size.metric.v.add(&s->head_virt_size.metric, sz);
    s->crlf_stored.metric.v.add(&s->crlf_stored.metric, file_size == sz);
  }

  return 0;
}

struct scanner scanner_class_mail = {
  .name = "mail file",
  .v = {
    .alloc = scanner_mail_alloc,
    .init = scanner_mail_init,
    .deinit = scanner_mail_deinit,
    .autodetect = scanner_mail_autodetect,
    .scan = scanner_mail_scan,
  },
};

