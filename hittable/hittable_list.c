#include "core/dyn_array.h"
#include "core/ray.h"
#include "hittable.h"

bool hittables_hit(DynArray *hittables, Ray ray, double ray_tmin,
                   double ray_tmax, HitRecord *rec) {

  HitRecord temp_rec;
  bool hit_anything = false;
  double closest_so_far = ray_tmax;

  for (int i = 0; i < dynarray_size(hittables); i++) {
    Hittable *h = (Hittable *)dynarray_get(hittables, i);
    if (h->hit(h, ray, ray_tmin, ray_tmax, &temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
  }

  return hit_anything;
}