#include "stdio.h"

void update_progress_bar(int current, int total) {
  int bar_width = 50;
  float progress = (float)current / total;
  int pos = (int)(bar_width * progress);

  printf("\r[");
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos)
      printf("=");
    else if (i == pos)
      printf(">");
    else
      printf(" ");
  }
  printf("] %3d%%", (int)(progress * 100));
  fflush(stdout);

  if (current == total) {
    printf("\n");
  }
}

void update_simple_progress_bar(int current, int total) {
  int percent = (int)(100.0 * current / total);
  printf("\rRendering: %3d%%", percent);
  fflush(stdout);
  if (current == total) {
    printf("\n");
  }
}