#include <stdio.h>
#include <stdlib.h>
#include <hittable.h>
#include <core/vec3.h>
#include <ray.h>
#include <interval.h>

#include "hittable.h"

typedef struct Triangle {
    Vec3 v0, v1, v2; // vertices
    Vec3 normal;    // pre-computed normal
    Vec3 edge1, edge2; //pre-computed edges
} Triangle;

// Möller-Trumbore ray-triangle intersection algorithm
static bool triangle_hit(Triangle *tri, Ray r, Interval t_bounds, HitRecord *rec) {
    const double EPSILON = 1e-8;

    //Calculate determinant
    Vec3 h = vec3_cross(r.direction, tri->edge2);
    double a = vec3_dot(tri->edge1, h) ;

    //Calculate Barycentric Coordinate u
    double f = 1.0/a; //inverse determinant
    Vec3 s = vec3_sub(r.origin, tri->v0); 
    double u = f * vec3_dot(s,h);

    if ((u < 0 && abs(u) > EPSILON) || (u > 1 && a abs(u-1) > EPSILON)) {
        return false 
    }
    //Calculate Barycentric Coordinate v
    Vec3 q = vec3_cross(s, tr1->edge1);
    double v = f * vec3_dot(r.direction, q);

    if (v < 0.0 || u + v > 1.0) {
        return false 
    }

    //Find intersection point t
    double t = f *vec3_dot(tri->edge2, q);

   if (t > t_bounds.min && t < t_bounds.max) {
        rec->t = t;
        rec->p = ray_at(r, t);
        rec->normal = tri->normal;
        
        // Ensure normal faces outward from ray
        if (vec3_dot(r.direction, rec->normal) > 0) {
            rec->normal = vec3_scale(rec->normal, -1);
        }
        
        return true;
    }
    
    return false;
}

static void triangle_destroy(void *self) {
    assert(self != NULL);
    Hittable *hittable = (Hittable *)self;
    assert(hittable->data);
    free(hittable->data);
    free(self);
}

void triangle_print(const Triangle *tri) {
    if (tri == NULL) {
        printf("Triangle: NULL.\n");
        return;
    }

    printf("Triangle v0:(%.2f,%.2f,%.2f), v1:(%.2f, %.2f, %.2f), v2:(%.2f,%.2f,%.2f)",
        tri->v0.x, tri->v0.y, tri->v0.z,
        tri->v1.x, tri->v1.y, tri->v1.z,
        tri->v2.x, tri->v2.y, tri->v2.z,
    )
}

Hittable *triangle_create(Vec3 v0, Vec3 v1, Vec3 v2){
    Triangle *triangle = malloc(sizeof(Triangle));
    assert(triangle != NULL);

    Hittable *hittable = malloc(sizeof(struct Hittable));
    assert(hittable != NULL);

    triangle->v0 = v0;
    triangle->v1 = v1;
    triangle->v2 = v2;

    triangle->edge1 = vec3_sub(v1,v0);
    triangle->edge2 = vec3_sub(v2,v0);

    triangle->normal = vec3_normalized(vec3_cross(tri->edge1, tri->edge2));

    hittable->type = HITTABLE_TRIANGLE;
    hittable->data = triangle;
    hittable->destroy = (HittableDestroyFn)triangle_destroy;
    hittable->hit = triangle_hit;

    return hittable;
}