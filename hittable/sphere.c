#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "sphere.h"
#include "core/interval.h"
#include "material/material.h"
#include "hittable.h"
#include "hit_record.h"

typedef struct Sphere {
  Vec3 center_start;
  Vec3 center_end;
  double radius;
  bool is_moving;
} Sphere;

static void get_sphere_uv(const Vec3* p, double* u, double* v) {
  double theta = acos(-p->y);
  double phi = atan2(-p->z, p->x) + M_PI;
    
  *u = phi / (2 * M_PI);
  *v = theta / M_PI;
}

static Vec3 sphere_center_at_time(const Sphere* sphere, double time) {
    if (!sphere->is_moving) {
        return sphere->center_start;  
    }

    Vec3 motion = vec3_sub(sphere->center_end, sphere->center_start);
    return vec3_add(sphere->center_start, vec3_scale(motion, time));
}

static bool sphere_hit(const Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
  assert(self != NULL);
  assert(rec != NULL);

  const Sphere *sphere = (const Sphere *)self->data;

  Vec3 current_center = sphere_center_at_time(sphere, ray.time);

  Vec3 oc = vec3_sub(current_center, ray.origin);
  double a = vec3_length_squared(ray.direction);
  double h = vec3_dot(ray.direction, oc);
  double c = vec3_length_squared(oc) - sphere->radius * sphere->radius;

  double discriminant = h * h - a * c;

  if (discriminant < 0) {
    return false;
  }
  double disc_sqrtd = sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range
  double root = (h - disc_sqrtd) / a;
  if (!interval_surrounds(t_bounds, root)) {
    root = (h + disc_sqrtd) / a;
    if (!interval_surrounds(t_bounds, root))
      return false;
  }
  
  rec->mat = self->mat;
  rec->t = root;
  rec->p = ray_at(ray, rec->t);
  Vec3 outward_normal =
      vec3_divs(vec3_sub(rec->p, current_center), sphere->radius);
  hitrec_set_face_normal(rec, ray, outward_normal);

  get_sphere_uv(&outward_normal, &rec->u, &rec->v);

  return true;
}

static void sphere_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

Hittable *sphere_create(Vec3 center, double radius, Material *mat) {
    assert(radius > 0);
    Hittable *hittable = malloc(sizeof(struct Hittable));
    assert(hittable != NULL);
    Sphere *sphere_data = malloc(sizeof(struct Sphere));
    assert(sphere_data != NULL);
    
    sphere_data->center_start = center;  
    sphere_data->center_end = center;    
    sphere_data->radius = radius;
    sphere_data->is_moving = false;    
    
    hittable->type = HITTABLE_SPHERE;
    hittable->hit = sphere_hit;
    hittable->destroy = (HittableDestroyFn)sphere_destroy;
    hittable->mat = mat;
    hittable->data = sphere_data;
    return hittable;
}

Hittable *sphere_create_moving(Vec3 center_start, Vec3 center_end, double radius, Material *mat) {
    assert(radius > 0);
    Hittable *hittable = malloc(sizeof(struct Hittable));
    assert(hittable != NULL);
    Sphere *sphere_data = malloc(sizeof(struct Sphere));
    assert(sphere_data != NULL);
    
    sphere_data->center_start = center_start;
    sphere_data->center_end = center_end;
    sphere_data->radius = radius;
    sphere_data->is_moving = true;
    
    hittable->type = HITTABLE_SPHERE;
    hittable->hit = sphere_hit;
    hittable->destroy = (HittableDestroyFn)sphere_destroy;
    hittable->mat = mat;
    hittable->data = sphere_data;
    return hittable;
}

void sphere_print(const Hittable *hittable) {
    if (hittable == NULL || hittable->type != HITTABLE_SPHERE) {
        printf("Sphere: Invalid or NULL\n");
        return;
    }
    const Sphere *sphere = (const Sphere *)hittable->data;
    if (sphere->is_moving) {
        printf("Moving Sphere { center_start: (%.3f, %.3f, %.3f), center_end: (%.3f, %.3f, %.3f), radius: %.3f }\n",
               sphere->center_start.x, sphere->center_start.y, sphere->center_start.z,
               sphere->center_end.x, sphere->center_end.y, sphere->center_end.z,
               sphere->radius);
    } 
    else {
        printf("Sphere { center: (%.3f, %.3f, %.3f), radius: %.3f }\n",
               sphere->center_start.x, sphere->center_start.y, sphere->center_start.z, sphere->radius);
    }
}
