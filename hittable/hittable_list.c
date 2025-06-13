#include "core/dyn_array.h"
#include "core/ray.h"
#include "core/interval.h"
#include "hittable.h"
#include "hit_record.h"

bool hittables_hit(DynArray *hittables, Ray ray, Interval t_bounds, HitRecord *rec) {
  HitRecord temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_bounds.max;

  for (int i = 0; i < dynarray_size(hittables); i++) {
    Hittable *h = (Hittable *)dynarray_get(hittables, i);
    if (h->hit(h, ray, interval_make(t_bounds.min, closest_so_far), &temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
  }

  return hit_anything;
}
