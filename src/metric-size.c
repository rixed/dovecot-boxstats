#include <stdio.h>
#include "boxstats.h"
#include "metric-size.h"
#include "misc.h"

struct metric metric_class_size;

void metric_size_init(struct metric_size *m)
{
  m->metric = metric_class_size;
  metric_init(&m->metric);
  m->sum = 0;
  FOR_EACH_P(m->distrib_log2, d) *d = 0; END_FOR_P
}

static void metric_size_add(struct metric *metric, size_t sz)
{
  struct metric_size *m = (struct metric_size *)metric; // DOWNCAST?
  metric_add(metric, sz);
  if (m->sum == 0) {
    m->min = m->max = sz;
  } else {
    if (sz > m->max) m->max = sz;
    if (sz < m->min) m->min = sz;
  }
  m->sum += sz;

  size_t S = 1;
  for (unsigned d = 0; d < SIZEOF_ARRAY(m->distrib_log2); d ++) {
    if (sz < S) {
      m->distrib_log2[d] ++;
      break;
    }
    S <<= 1;
  }
}

static void metric_size_deinit(struct metric *metric)
{
  struct metric_size *m = (struct metric_size *)metric; // DOWNCAST?

  if (m->metric.count == 0) return;

  unsigned max_d, min_d;
  for (max_d = SIZEOF_ARRAY(m->distrib_log2) - 1; max_d > 0; max_d --) {
    if (m->distrib_log2[max_d] != 0) break;
  }
  for (min_d = 0; min_d < max_d; min_d ++) {
    if (m->distrib_log2[min_d] != 0) break;
  }

  printf("%u messages of total size %zu (avg: %zu)\n",
         m->metric.count, m->sum, (m->sum + m->metric.count - 1) / m->metric.count);

  /* Print distribution of sizes */
  for (unsigned d = min_d; d <= max_d; d ++) {
    size_t const max_S = 1 << d;
    size_t const min_S = max_S >> 1;
    printf("%zu-%zu bytes: %zu\n", min_S, max_S, m->distrib_log2[d]);
  }
}

struct metric metric_class_size = {
  .name = "size",
  .v = {
    .deinit = metric_size_deinit,
    .add = metric_size_add,
  },
};
