#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "quad.h"
#include "core/vec3.h"
#include "core/interval.h"
#include "material/material.h"
#include "hittable.h"
#include "hit_record.h"
#include "core/dyn_array.h"

typedef struct Quad {
    Vec3 Q;   
    Vec3 u;
    Vec3 v;
    Vec3 normal;
    Vec3 w;  
    double D;
} Quad;

static bool quad_hit(const Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
  assert(self != NULL);
  assert(rec != NULL);

  const Quad *q = (const Quad *)self->data;

  double denom = vec3_dot(q->normal, ray.direction);
   if (fabs(denom) < DBL_EPSILON) {
    return false;
  }

  double t = (q->D - vec3_dot(q->normal, ray.origin)) / denom;
  if (!interval_surrounds(t_bounds, t)) {
        return false; 
  }

  Vec3 intersection = ray_at(ray, t);

  Vec3 p = vec3_sub(intersection, q->Q);
  double alpha = vec3_dot(q->w, vec3_cross(p, q->v));
  double beta  = vec3_dot(q->w, vec3_cross(q->u, p));

  if (alpha < 0.0 || alpha > 1.0 || beta < 0.0 || beta > 1.0) {
        return false;
  }
  
  rec->t = t;
  rec->p = intersection;
  rec->mat = self->mat;
  hitrec_set_face_normal(rec, ray, q->normal);
  return true;
}

static void quad_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  free(hittable->data);
  free(self);
}

Hittable *quad_create(Vec3 Q, Vec3 u, Vec3 v, Material *mat) {

  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  Quad *quad_data = malloc(sizeof(struct Quad));
  assert(quad_data != NULL);

  quad_data->Q = Q;
  quad_data->u = u;
  quad_data->v = v;

  Vec3 n = vec3_cross(u, v);
  quad_data->normal = vec3_normalized(n);
  quad_data->D = vec3_dot(quad_data->normal, Q);
  quad_data->w = vec3_divs(n, vec3_dot(n, n));

  hittable->type = HITTABLE_QUAD;
  hittable->hit = quad_hit;
  hittable->destroy = (HittableDestroyFn)quad_destroy;
  hittable->mat = mat;
  hittable->data = quad_data;

  return hittable;
}

void quad_print(const Hittable *hittable) {
    if (hittable == NULL || hittable->type != HITTABLE_QUAD) {
        printf("Quad: Invalid or NULL\n");
        return;
    }
    const Quad *quad = (const Quad *)hittable->data;
    printf(
        "Quad { Q: (%.3f, %.3f, %.3f), u: (%.3f, %.3f, %.3f), v: (%.3f, %.3f, %.3f) }\n",
        quad->Q.x, quad->Q.y, quad->Q.z,
        quad->u.x, quad->u.y, quad->u.z,
        quad->v.x, quad->v.y, quad->v.z
    );
}

DynArray* box_create(const Vec3 a, const Vec3 b, Material* mat) {
    DynArray* sides = dynarray_create(6, NULL, (GDestroyFn)hittable_destroy);
    
    Vec3 min = (Vec3){
        fmin(a.x, b.x),
        fmin(a.y, b.y),
        fmin(a.z, b.z)
    };
    
    Vec3 max = (Vec3){
        fmax(a.x, b.x),
        fmax(a.y, b.y),
        fmax(a.z, b.z)
    };
    
    Vec3 dx = (Vec3){max.x - min.x, 0, 0};
    Vec3 dy = (Vec3){0, max.y - min.y, 0};
    Vec3 dz = (Vec3){0, 0, max.z - min.z};
    
    Hittable* front = quad_create(
        (Vec3){min.x, min.y, max.z},
        dx,
        dy,
        mat
    );
    dynarray_push(sides, front);
    
    Hittable* right = quad_create(
        (Vec3){max.x, min.y, max.z},
        vec3_negate(dz),
        dy,
        mat
    );
    dynarray_push(sides, right);
    
    Hittable* back = quad_create(
        (Vec3){max.x, min.y, min.z},
        vec3_negate(dx),
        dy,
        mat
    );
    dynarray_push(sides, back);
    
    Hittable* left = quad_create(
        (Vec3){min.x, min.y, min.z},
        dz,
        dy,
        mat
    );
    dynarray_push(sides, left);
    
    Hittable* top = quad_create(
        (Vec3){min.x, max.y, max.z},
        dx,
        vec3_negate(dz), 
        mat
    );
    dynarray_push(sides, top);
    
    Hittable* bottom = quad_create(
        (Vec3){min.x, min.y, min.z},
        dx,
        dz,
        mat
    );
    dynarray_push(sides, bottom);
    
    return sides;
}
