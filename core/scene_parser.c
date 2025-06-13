#include "scene_parser.h"

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 20

typedef enum {
    TOPLEVEL_STATE,
    CAMERA_STATE,
    SPHERE_STATE,
    PLANE_STATE,
} ParserState;

static int tokenize(char *line, char *tokens[]) {
    int num_toks = 0;
    char *saveptr = NULL;
    char *token = strtok_r(line, " \t\n", &saveptr);
    
    while (token && count < MAX_TOKENS) {
        tokens[num_toks++] = token;
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    return num_toks;
}

void parse_scene(const char *filename, DynArray *hittable_world) {
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    ParserState state = TOPLEVEL_STATE;

    char line[MAX_LINE];
    char *tokens[MAX_TOKENS];

    Vec3 center;
    double radius;

    Vec3

    while (fgets(line, MAX_LINE, file)) {
        if (line[0] == '\n') {
            continue;
        }

        if (strchr(line, '}')) {
            switch (state) {
                case SPHERE_STATE:
                    dynarray_push(hittable_world, sphere_create(center, radius));
                    break;
                case PLANE_STATE:
                    dynarray_push(world, plane_create(point, normal));
                    break;
            }
            state = TOPLEVEL_STATE;
            continue;
        }

        int num_toks = tokenize(line, tokens);
        if (count == 0) continue;

        if (state == STATE_TOPLEVEL) {
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
                        position.x = atof(tokens[1]);
                        position.y = atof(tokens[2]);
                        position.z = atof(tokens[3]);
                    }
                    else if (strcmp(tokens[0], "radius") == 0 && num_toks == 2) {
                        radius = atof(tokens[1]);
                    }
                    break
            }

        }
        fclose(file);
    }

    ParserState state = STATE_TOPLEVEL;
}