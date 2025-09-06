#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include "../core/dyn_array.h"
#include "../app/scene.h"

void parse_scene(const char *filename, Scene *scene, Camera *out_cam);

#endif