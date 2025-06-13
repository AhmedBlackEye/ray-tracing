#include "scene_parser.h"

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 20

typedef enum {
    TOPLEVEL_STATE,
    CAMERA_STATE,
    SPHERE_STATE,
    PLANE_STATE
} ParserState;

static int tokenize(char *line, char *tokens[]) {
    int num_toks = 0;
    char *saveptr = NULL;
    char *token = strtok_r(line, " \t\n", &saveptr);
    
    while (token && num_toks < MAX_TOKENS) {
        tokens[num_toks++] = token;
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    return num_toks;
}

void parse_scene(const char *filename, DynArray *hittable_world, FILE *out_file) {
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    ParserState state = TOPLEVEL_STATE;

    char line[MAX_LINE_LENGTH];
    char *tokens[MAX_TOKENS];

    Vec3 center;
    double radius;

    Vec3 normal;
    Vec3 point;

    double aspect_ratio_width;
    double aspect_ratio_height;
    double aspect_ratio;
    int width;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (line[0] == '\n') {
            continue;
        }

        if (strchr(line, '}')) {
            switch (state) {
                case SPHERE_STATE:
                    dynarray_push(hittable_world, sphere_create(center, radius));
                    break;
                case PLANE_STATE:
                    dynarray_push(hittable_world, plane_create(point, normal));
                    break;
                case CAMERA_STATE:
                    Camera cam = camera_make(width, aspect_ratio);
                    camera_render(&cam, hittable_world, out_file);
                    break;
            }
            state = TOPLEVEL_STATE;
            continue;
        }

        int num_toks = tokenize(line, tokens);
        if (num_toks == 0) continue;

        if (state == TOPLEVEL_STATE) {
            if (strcmp(tokens[0], "camera") == 0) {
                state = CAMERA_STATE;
            }
            else if (strcmp(tokens[0], "sphere") == 0) {
                state = SPHERE_STATE;
            }
            else if (strcmp(tokens[0], "plane") == 0) {
                state = PLANE_STATE;
            }
        }
        else {
            switch (state) {
                case SPHERE_STATE:
                    if (strcmp(tokens[0], "center") == 0 && num_toks == 4) {
                        center.x = atof(tokens[1]);
                        center.y = atof(tokens[2]);
                        center.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "radius") == 0 && num_toks == 2) {
                        radius = atof(tokens[1]);
                    }
                    break;
                case PLANE_STATE:
                    if (strcmp(tokens[0], "point") == 0 && num_toks == 4) {
                        point.x = atof(tokens[1]);
                        point.y = atof(tokens[2]);
                        point.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "normal") == 0 && num_toks == 4) {
                        normal.x = atof(tokens[1]);
                        normal.y = atof(tokens[2]);
                        normal.z = atof(tokens[3]);
                    }
                    break;
                case CAMERA_STATE:
                    if (strcmp(tokens[0], "aspect_ratio") == 0 && num_toks == 3) {
                        aspect_ratio_width = atof(tokens[1]);
                        aspect_ratio_height = atof(tokens[2]);
                        aspect_ratio = aspect_ratio_width / aspect_ratio_height;
                    }
                    else if (strcmp(tokens[0], "width") == 0 && num_toks == 2) {
                        width = atoi(tokens[1]);
                    }
            }

        }
    }
    fclose(file);
}