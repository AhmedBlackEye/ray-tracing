#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include "dyn_array.h"

void parse_scene(const char *filename, DynArray *hittable_world, Camera *out_cam);

#endif