#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bvh_node.h"
#include "core/aabb.h"
#include "core/dyn_array.h"
#include "core/interval.h"
#include "core/ray.h"
#include "hit_record.h"
#include "hittable.h"

#include "material/material.h"

typedef struct BVHNode {
  Hittable *left;
  Hittable *right;
} BVHNode;

bool bvhnode_hit(Hittable *self, Ray ray, Interval t_bounds, HitRecord *rec) {
  assert(self != NULL);
  assert(rec != NULL);

  if (!aabb_hit(&self->bbox, ray, &t_bounds)) {
    return false;
  }

  BVHNode *node = self->data;
  bool hit_left = node->left->hit(node->left, ray, t_bounds, rec);
  Interval new_interval =
      interval_make(t_bounds.min, hit_left ? rec->t : t_bounds.max);
  bool hit_right = node->right->hit(node->right, ray, new_interval, rec);
  return hit_left | hit_right;
}

static void bvhnode_destroy(void *self) {
  assert(self != NULL);
  Hittable *hittable = (Hittable *)self;
  assert(hittable->data);
  BVHNode *node = hittable->data;

  // Only destroy if it's a BVH node (not an original scene object)
  if (node->left && node->left->type == HITTABLE_BVHNODE) {
    node->left->destroy(node->left);
  }

  // Check if right is different from left AND is a BVH node
  if (node->right && node->right != node->left &&
      node->right->type == HITTABLE_BVHNODE) {
    node->right->destroy(node->right);
  }

  free(node);
  free(self);
}

// Fixed comparison functions - need to return int, not bool
static int box_x_compare(const void *a, const void *b) {
  Hittable *ha = *(Hittable **)a;
  Hittable *hb = *(Hittable **)b;

  if (ha->bbox.x.min < hb->bbox.x.min)
    return -1;
  if (ha->bbox.x.min > hb->bbox.x.min)
    return 1;
  return 0;
}

static int box_y_compare(const void *a, const void *b) {
  Hittable *ha = *(Hittable **)a;
  Hittable *hb = *(Hittable **)b;

  if (ha->bbox.y.min < hb->bbox.y.min)
    return -1;
  if (ha->bbox.y.min > hb->bbox.y.min)
    return 1;
  return 0;
}

static int box_z_compare(const void *a, const void *b) {
  Hittable *ha = *(Hittable **)a;
  Hittable *hb = *(Hittable **)b;

  if (ha->bbox.z.min < hb->bbox.z.min)
    return -1;
  if (ha->bbox.z.min > hb->bbox.z.min)
    return 1;
  return 0;
}

static Hittable *bvhnode_create_helper(DynArray *objects, size_t start,
                                       size_t end) {
  Hittable *hittable = malloc(sizeof(struct Hittable));
  assert(hittable != NULL);

  BVHNode *node = malloc(sizeof(struct BVHNode));
  assert(node != NULL);

  // Calculate bounding box for this node
  hittable->bbox = aabb_empty();
  for (size_t obj_index = start; obj_index < end; obj_index++) {
    Hittable *h = dynarray_get(objects, obj_index);
    hittable->bbox = aabb_surrounding_box(&hittable->bbox, &h->bbox);
  }

  int axis = aabb_longest_axis(&hittable->bbox);
  size_t object_span = end - start;

  GCmp comparator;
  if (axis == 0) {
    comparator = box_x_compare;
  } else if (axis == 1) {
    comparator = box_y_compare;
  } else {
    comparator = box_z_compare;
  }

  if (object_span == 1) {
    // Single object - both children point to same original object
    // We DO NOT own these - they belong to the scene
    node->left = node->right = dynarray_get(objects, start);
  } else if (object_span == 2) {
    // Two objects - point to original objects
    // We DO NOT own these - they belong to the scene
    Hittable *a = dynarray_get(objects, start);
    Hittable *b = dynarray_get(objects, start + 1);
    if (comparator(&a, &b) > 0) {
      node->left = b;
      node->right = a;
    } else {
      node->left = a;
      node->right = b;
    }
  } else {
    // Multiple objects - create child BVH nodes
    // We OWN these because we created them
    dynarray_partial_sort(objects, start, end, comparator);

    size_t mid = start + object_span / 2;
    node->left = bvhnode_create_helper(objects, start, mid);
    node->right = bvhnode_create_helper(objects, mid, end);
  }

  hittable->type = HITTABLE_BVHNODE;
  hittable->hit = (HitFn)bvhnode_hit;
  hittable->destroy = (HittableDestroyFn)bvhnode_destroy;
  hittable->mat = NULL;
  hittable->data = node;

  return hittable;
}

Hittable *bvhnode_create(Hittable *hittable_list) {
  assert(hittable_list->type == HITTABLE_LIST);
  DynArray *objects = hittable_list->data;

  printf("\n=== CREATING BVH ===\n");
  printf("Input objects: %d\n", dynarray_size(objects));

  if (dynarray_size(objects) == 0) {
    printf("ERROR: No objects to create BVH from!\n");
    return NULL;
  }

  Hittable *bvh = bvhnode_create_helper(objects, 0, dynarray_size(objects));

  printf("BVH created successfully\n");
  printf("===================\n\n");

  return bvh;
}

void bvhnode_print(const Hittable *hittable) {
  if (hittable == NULL || hittable->type != HITTABLE_BVHNODE) {
    printf("BVH Node: Invalid or NULL\n");
    return;
  }
  BVHNode *node = hittable->data;
  printf("BVH Node: { left: %p, right: %p }\n", (void *)node->left,
         (void *)node->right);
}