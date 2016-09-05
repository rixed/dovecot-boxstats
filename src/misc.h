#ifndef MISC_H_20160831
#define MISC_H_20160831

#define SIZEOF_ARRAY(x) (sizeof(x) / sizeof(*x))

#define FOR_EACH_P(things, thing) do { \
  for (unsigned thing ## _count = 0; \
       thing ## _count < SIZEOF_ARRAY(things); \
       thing ## _count ++) { \
    typeof(*things) *thing = (things) + thing ## _count; \
    do
#define END_FOR_P while (0); } } while (0);

#define FOR_EACH(things, thing) do { \
  FOR_EACH_P(things, thing ## p) { \
    typeof(*(things)) thing = *(thing ## p); \
    do
#define END_FOR while (0); } END_FOR_P } while (0);

#endif
