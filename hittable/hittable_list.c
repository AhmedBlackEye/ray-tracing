#include "core/dyn_array.h"
#include "core/ray.h"
#include "core/interval.h"
#include "hittable.h"
#include "hit_record.h"

typedef struct HittableList {
    AABB box;
    DynArray objects;
    HitFn hit;
} HittableList;

bool hittablelist_hit(HittableList *hittables, Ray ray, Interval t_bounds, HitRecord *rec) {
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


 HittableList *hittable_list_empty() {
    HittableList new = malloc(sizeof(struct HittableList));
    assert( new != NULL );
    new->box = aabb_empty();
    new->objects = dynarray_create(8, (GPrintFn)hittable_print, (GDestroyFn)hittable_destroy);
    new->hit = hittables_hit;
    return new;
}

