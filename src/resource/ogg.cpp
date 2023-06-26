#include "ogg.h"
#include <iostream>
#include <ogg/ogg.h>

Ogg::Ogg(const char* path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    printf("Failed to open file\n");
    return;
  }

  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);

  buffer = malloc(size);
  fread(buffer, 1, size, f);
  fclose(f);
}

Ogg::~Ogg() {
  if (buffer != NULL)
    free(buffer);
}