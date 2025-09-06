#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

/* Internal helper macro - not for direct use */
#define DEBUG_REPORT(tag, trigger, fmt, ...)                             \
    do                                                                   \
    {                                                                    \
        if (trigger)                                                     \
        {                                                                \
            fprintf(stderr, "[%s] %s:%d (%s): " fmt "\n",                \
                    (tag), __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
            abort();                                                     \
        }                                                                \
    } while (0)

/* Assertion (compiled out in release builds) */
#ifndef NDEBUG
#define DEV_ASSERT(cond, fmt, ...) \
    DEBUG_REPORT("DEBUG ASSERT FAILED", !(cond), fmt, ##__VA_ARGS__)
#else
#define DEV_ASSERT(cond, fmt, ...) ((void)0)
#endif

/* Unconditional termination (compiled out in release builds) */
#ifndef NDEBUG
#define DEV_PANIC(cond, fmt, ...) \
    DEBUG_REPORT("DEBUG ASSERT FAILED", !(cond), fmt, ##__VA_ARGS__)
#else
#define DEV_PANIC(cond, fmt, ...) ((void)0)
#endif

/* Unconditional termination */
#define PANIC(fmt, ...) \
    DEBUG_REPORT("PANIC", 1, fmt, ##__VA_ARGS__)

/* Conditional termination (triggers if condition is true) */
#define PANIC_IF(condition, fmt, ...) \
    DEBUG_REPORT("PANIC", (condition), fmt, ##__VA_ARGS__)

#endif
