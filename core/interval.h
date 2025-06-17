#ifndef INTERVAL_H
#define INTERVAL_H

#include <math.h>
#include <stdbool.h>

typedef struct Interval {
  double min;
  double max;
} Interval;

// Returns an empty interval
static inline Interval interval_empty(void) {
  return (Interval){+INFINITY, -INFINITY};
}

// Returns a full interval
static inline Interval interval_universe(void) {
  return (Interval){-INFINITY, +INFINITY};
}

// Constructor with explicit bounds
static inline Interval interval_make(double min, double max) {
  return (Interval){min, max};
}

static inline Interval interval_enclose(Interval a, Interval b) {
  double min = a.min <= b.min ? a.min : b.min;
  double max = a.max >= b.max ? a.max : b.max;
  return (Interval){min, max};
}

// Returns the size of the interval
static inline double interval_size(Interval i) { return i.max - i.min; }

// Returns true if x is in [min, max]
static inline bool interval_contains(Interval i, double x) {
  return i.min <= x && x <= i.max;
}

// Returns true if x is in (min, max)
static inline bool interval_surrounds(Interval i, double x) {
  return i.min < x && x < i.max;
}

static inline double interval_clamp(Interval i, double x) {
  if (x < i.min)
    return i.min;
  if (x > i.max)
    return i.max;
  return x;
}

static inline Interval interval_expand(Interval i, double delta) {
  double padding = delta / 2;
  return interval_make(i.min - padding, i.max + padding);
}

#endif // INTERVAL_H
