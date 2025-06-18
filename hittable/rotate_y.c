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

    Vec3 origin = (Vec3){
        r->cos_theta * ray.origin.x - r->sin_theta * ray.origin.z,
        ray.origin.y,
        r->sin_theta * ray.origin.x + r->cos_theta * ray.origin.z
    };
    
    Vec3 direction = (Vec3){
        r->cos_theta * ray.direction.x - r->sin_theta * ray.direction.z,
        ray.direction.y,
        r->sin_theta * ray.direction.x + r->cos_theta * ray.direction.z
    };
    
    Ray rotated_ray;
    rotated_ray.origin = origin;
    rotated_ray.direction = direction;
    rotated_ray.time = ray.time;
    
    if (!r->object->hit(r->object, rotated_ray, t_bounds, rec)) {
        return false;
    }
    
    rec->p = (Vec3){
        r->cos_theta * rec->p.x + r->sin_theta * rec->p.z,
        rec->p.y,
        -r->sin_theta * rec->p.x + r->cos_theta * rec->p.z
    };
    
    rec->normal = (Vec3){
        r->cos_theta * rec->normal.x + r->sin_theta * rec->normal.z,
        rec->normal.y,
        -r->sin_theta * rec->normal.x + r->cos_theta * rec->normal.z
    };
    
    return true;
}

static void rotate_y_destroy(void *self) {
    assert(self != NULL);
    Hittable *hittable = (Hittable *)self;
    assert(hittable->data);
    
    free(hittable->data);
    free(self);
}

Hittable *rotate_y_create(Hittable* object, double angle_degrees) {
    assert(object != NULL);
    
    Hittable *hittable = malloc(sizeof(struct Hittable));
    assert(hittable != NULL);
    RotateY *rotate_data = malloc(sizeof(struct RotateY));
    assert(rotate_data != NULL);
    
    double radians = angle_degrees * M_PI / 180.0;
    
    rotate_data->object = object;
    rotate_data->sin_theta = sin(radians);
    rotate_data->cos_theta = cos(radians);
    
    hittable->type = HITTABLE_ROTATE_Y;
    hittable->hit = rotate_y_hit;
    hittable->destroy = (HittableDestroyFn)rotate_y_destroy;
    hittable->mat = object->mat;  
    hittable->data = rotate_data;
    
    return hittable;
}

void rotate_y_print(const Hittable *hittable) {
    if (hittable == NULL || hittable->type != HITTABLE_ROTATE_Y) {
        printf("RotateY: Invalid or NULL\n");
        return;
    }
    const RotateY *rotate = (const RotateY *)hittable->data;

    double angle_radians = atan2(rotate->sin_theta, rotate->cos_theta);
    double angle_degrees = angle_radians * 180.0 / M_PI;
    printf("RotateY { angle: %.1f degrees }\n", angle_degrees);
}