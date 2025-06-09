#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define HEIGHT 256
#define WIDTH 256

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file> [output_file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *out_file = fopen(argv[1], "w");
    if (!out_file)
    {
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }

    fprintf(out_file, "P3\n%d %d\n255\n", WIDTH, HEIGHT);
    for(int j=0; j < HEIGHT; j++) {
        for(int i=0; i < WIDTH; i++) {
            int r = (j * 255) / (HEIGHT - 1);
            int g = (i * 255) / (WIDTH - 1);
            int b = 0;
            
            fprintf(out_file, "%d %d %d\n", r, g, b);
        }
    }

}