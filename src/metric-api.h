#ifndef METRIC_API_H_20160905
#define METRIC_API_H_20160905

struct metric;

struct metric_vfuncs {
  void (*deinit)(struct metric *);
  void (*add)(struct metric *, size_t);
};

struct metric {
  char const *name;
  struct metric_vfuncs v;
  unsigned count;
};

void metric_init(struct metric *);
void metric_add(struct metric *, size_t);

#endif
