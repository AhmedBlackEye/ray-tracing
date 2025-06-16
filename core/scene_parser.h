#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include "dyn_array.h"
#include "scene.h"

void parse_scene(const char *filename, Scene *scene, Camera *out_cam);

#endif