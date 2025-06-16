#include "diffuse_light.h"
#include "texture/solid_color.h"    
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct DiffuseLight {
    Texture *tex; 
} DiffuseLight;

static bool diffuse_light_scatter(Material *mat, Ray ray_in, HitRecord *rec,
                                  Color *attenuation, Ray *scattered) {
    return false;
}

static void diffuse_light_destroy(void *self) {
  assert(self != NULL);
  Material *mat = (Material *)self;
  if (mat->data)
    free(mat->data);
  free(self);
}

void diffuse_light_print(const Material *self) {
  if (self == NULL || self->type != MATERIAL_DIFFUSE_LIGHT) {
    printf("Diffuse Light: Invalid or NULL\n");
    return;
  }
    const DiffuseLight *diff_l = (const DiffuseLight *)self->data;
    printf("Diffuse Light { texture: %p }\n", (void*)diff_l->tex);
}

Color diffuse_light_emitted(Material *self, double u, double v, const Vec3 *p) {
    if (!self || self->type != MATERIAL_DIFFUSE_LIGHT) {
        return vec3_zero();
    }
    
    DiffuseLight *data = (DiffuseLight*)self->data;
    return data->tex->value(data->tex, u, v, p);
}

// New texture-based constructor
Material *diffuse_light_create_texture(Texture* tex) {
    Material *mat = malloc(sizeof(struct Material));
    assert(mat != NULL);
    
    DiffuseLight *diff_l = malloc(sizeof(struct DiffuseLight));
    assert(diff_l != NULL);
    
    diff_l->tex = tex; 
    
    mat->type = MATERIAL_DIFFUSE_LIGHT;
    mat->scatter = (ScatterFn)diffuse_light_scatter;
    mat->destroy = (MaterialDestroyFn)diffuse_light_destroy;
    mat->emitted = diffuse_light_emitted;
    mat->data = diff_l;
    return mat;
}

Material *diffuse_light_create(Color emit_color) {
    SolidColor* sol_col = solid_color_create_albedo(&emit_color);
    return diffuse_light_create_texture((Texture*)sol_col);
}
