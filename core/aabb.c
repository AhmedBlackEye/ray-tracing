#include <stdbool.h>

#include "interval.h"
#include "vec3.h"
#include "ray.h"

bool aabb_hit(AABB *box, Ray *r, Interval *ray_t) {
    Vec3 *ray_origin = &r->origin;
    Vec3 *ray_direction = &r->direction;

    for (int axis = 0; axis < 3; axis++) {
        Interval *ax = axis_interval(box, axis);
        double adinv = 1.0 / vec3_axis(ray_direction, axis);
        
        double t0 = (ax->min - vec3_axis(ray_origin, axis)) * adinv;
        double t1 = (ax->max - vec3_axis(ray_origin, axis)) * adinv;

        if (t0 < t1) {
            if (t0 > ray_t->min) ray_t->min = t0;
            if (t1 < ray_t->max) ray_t->max = t1;
        } else {
            if (t1 > ray_t->min) ray_t->min = t1;
            if (t0 < ray_t->max) ray_t->max = t0;
        }

        if (ray_t->max <= ray_t->min) return false;
    }
    
    return true;
}
