#ifndef METRIC_SIZE_H_20160905
#define METRIC_SIZE_H_20160905
#include "metric-api.h"

struct metric_size {
  struct metric metric;
  size_t sum;
  size_t min, max;  // undef unless sum>0
  size_t distrib_log2[60];
};

void metric_size_init(struct metric_size *);

#endif
