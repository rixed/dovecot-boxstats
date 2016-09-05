#include <stdio.h>
#include "boxstats.h"
#include "metric-bool.h"
#include "misc.h"

struct metric metric_class_bool;

void metric_bool_init(struct metric_bool *m)
{
  m->metric = metric_class_bool;
  metric_init(&m->metric);
  m->sum = 0;
}

static void metric_bool_add(struct metric *metric, size_t sz)
{
  struct metric_bool *m = (struct metric_bool *)metric; // DOWNCAST?
  metric_add(&m->metric, sz);
}

static void metric_bool_deinit(struct metric *metric)
{
  struct metric_bool *m = (struct metric_bool *)metric; // DOWNCAST?

  if (m->metric.count == 0) return;

  printf("%zu messages out of %u where filling the condition (%3.1f%%)\n",
         m->sum, m->metric.count, 100. * m->sum / m->metric.count);
}

struct metric metric_class_bool = {
  .name = "bool",
  .v = {
    .deinit = metric_bool_deinit,
    .add = metric_bool_add,
  },
};
