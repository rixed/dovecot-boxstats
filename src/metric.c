#include "boxstats.h"
#include "metric-api.h"

// Base class

void metric_init(struct metric *m)
{
  m->count = 0;
}

void metric_add(struct metric *m, size_t s)
{
  (void)s;
  m->count ++;
}

