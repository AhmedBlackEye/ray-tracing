#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "translate.h"
#include "core/interval.h"
#include "hittable.h"
#include "hit_record.h"

typedef struct Translate {
    Hittable* object;
    Vec3 offset;
} Translate;

static bool translate_hit(const Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
    assert(self != NULL);
    assert(rec != NULL);

    const Translate *t = (const Translate *)self->data;
    
    Ray offset_ray;
    offset_ray.origin = vec3_sub(ray.origin, t->offset);
    offset_ray.direction = ray.direction;
    offset_ray.time = ray.time;
    
    if (!t->object->hit(t->object, offset_ray, t_bounds, rec)) {
        return false;
    }

    rec->p = vec3_add(rec->p, t->offset);
    
    return true;
}

static void translate_destroy(void *self) {
    assert(self != NULL);
    Hittable *hittable = (Hittable *)self;
    assert(hittable->data);
    
    free(hittable->data);
    free(self);
}

Hittable *translate_create(Hittable* object, Vec3 offset) {
    assert(object != NULL);
    
    Hittable *hittable = malloc(sizeof(struct Hittable));
    assert(hittable != NULL);
    Translate *translate_data = malloc(sizeof(struct Translate));
    assert(translate_data != NULL);
    
    translate_data->object = object;
    translate_data->offset = offset;
    
    hittable->type = HITTABLE_TRANSLATE;
    hittable->hit = translate_hit;
    hittable->destroy = (HittableDestroyFn)translate_destroy;
    hittable->mat = object->mat; 
    hittable->data = translate_data;
    
    return hittable;
}

void translate_print(const Hittable *hittable) {
    if (hittable == NULL || hittable->type != HITTABLE_TRANSLATE) {
        printf("Translate: Invalid or NULL\n");
        return;
    }
    const Translate *translate = (const Translate *)hittable->data;
    printf("Translate { offset: (%.3f, %.3f, %.3f) }\n",
           translate->offset.x, translate->offset.y, translate->offset.z);
}