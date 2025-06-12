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

#endif // INTERVAL_H
