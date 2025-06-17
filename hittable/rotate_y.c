#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "rotate_y.h"
#include "core/interval.h"
#include "hittable.h"
#include "hit_record.h"

typedef struct RotateY {
    Hittable* object;
    double sin_theta;
    double cos_theta;
} RotateY;

static bool rotate_y_hit(const Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
    assert(self != NULL);
    assert(rec != NULL);

    const RotateY *r = (const RotateY *)self->data;

    Vec3 origin = vec3_create(
        r->cos_theta * ray.origin.x - r->sin_theta * ray.origin.z,
        ray.origin.y,
        r->sin_theta * ray.origin.x + r->cos_theta * ray.origin.z
    );
    
    Vec3 direction = vec3_create(
        r->cos_theta * ray.direction.x - r->sin_theta * ray.direction.z,
        ray.direction.y,
        r->sin_theta * ray.direction.x + r->cos_theta * ray.direction.z
    );
    
    Ray rotated_ray;
    rotated_ray.origin = origin;
    rotated_ray.direction = direction;
    rotated_ray.time = ray.time;
    
    if (!r->object->hit(r->object, rotated_ray, t_bounds, rec)) {
        return false;
    }
    
    rec->p = vec3_create(
        r->cos_theta * rec->p.x + r->sin_theta * rec->p.z,
        rec->p.y,
        -r->sin_theta * rec->p.x + r->cos_theta * rec->p.z
    );
    
    rec->normal = vec3_create(
        r->cos_theta * rec->normal.x + r->sin_theta * rec->normal.z,
        rec->normal.y,
        -r->sin_theta * rec->normal.x + r->cos_theta * rec->normal.z
    );
    
    return true;
}

static void rotate_y_destroy(void *self) {
    assert(self != NULL);
    Hittable *hittable = (Hittable *)self;
    assert(hittable->data);
    
    free(hittable->data);
    free(self);
}