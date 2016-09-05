#ifndef METRIC_BOOL_H_20160905
#define METRIC_BOOL_H_20160905
#include "metric-api.h"

struct metric_bool {
  struct metric metric;
  size_t sum;
};

void metric_bool_init(struct metric_bool *);

#endif
