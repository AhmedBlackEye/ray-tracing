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
        tokens[count++] = token;
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    return count;
}

void parse_scene(const char *filename, DynArray *hittable_world) {
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    char line[MAX_LINE];
    char *tokens[MAX_TOKENS];

    while (fgets(line, MAX_LINE, file)) {
        if (line[0] == '\n') {
            continue;
        }

        if (strchr(line, '}')) {
            switch (state) {
                case SPHERE_STATE:
                    dynarray_add(hittable_world, sphere_create(center, radius));
                    break;
                case PLANE_STATE:
            }
        }

        int num_toks = tokenize(line, tokens);
    }

    ParserState state = STATE_TOPLEVEL;
}