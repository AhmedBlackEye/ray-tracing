
typedef void *GVal;
typedef void (*GPrintFn)(GVal value);
typedef void (*GDestroyFn)(GVal self);
typedef int (*GCmp)(const void *a, const void *b);
