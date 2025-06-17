#include "core/dyn_array.h"
#include "core/generic_types.h"
#include "core/interval.h"
#include "core/ray.h"
#include "hit_record.h"
#include "hittable.h"

bool hittablelist_hit(Hittable *self, Ray ray, Interval t_bounds,
                      HitRecord *rec) {
  HitRecord temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_bounds.max;

  DynArray *hittables = self->data;
  for (int i = 0; i < dynarray_size(hittables); i++) {
    Hittable *h = (Hittable *)dynarray_get(hittables, i);
    if (h->hit(h, ray, interval_make(t_bounds.min, closest_so_far),
               &temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      *rec = temp_rec;
    }
  }

  return hit_anything;
}

void hittablelist_destroy(Hittable *self) {
  assert(self);
  assert(self->data);

  free(self->data);
  free(self);
}

void hittablelist_add(Hittable *self, Hittable *new_hittable) {
  assert(self->type == HITTABLE_LIST);
  dynarray_push((DynArray *)self->data, new_hittable);
  self->bbox = aabb_surrounding_box(&self->bbox, &new_hittable->bbox);
}

Hittable *hittablelist_empty(void) {
  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  hittable->type = HITTABLE_LIST;
  hittable->hit = (HitFn)hittablelist_hit;
  hittable->destroy = (HittableDestroyFn)hittablelist_destroy;
  hittable->mat = NULL;
  hittable->bbox = aabb_empty();
  hittable->data = dynarray_create(8, (GPrintFn)hittable_print,
                                   (GDestroyFn)hittable_destroy);
  return hittable;
}

void hittablelist_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_LIST) {
    printf("Hittable List: Invalid or NULL\n");
    return;
  }
  printf("Hittable List: ");
  dynarray_print((DynArray *)hittable->data);
  putchar('\n');
}
