#ifndef MODEL_H
#define MODEL_H
#include <gctypes.h>

class Model {
private:
  void* display; // displaylist
  u32 size;

public:
  // TODO: add mesh data as first argument
  Model(const char* filename);
  ~Model();
  void render();
};

#endif // MODEL_H