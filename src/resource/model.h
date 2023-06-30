#ifndef MODEL_H
#define MODEL_H
#include <gctypes.h>
#include <fstream>

class Model {
private:
  void* display; // displaylist
  u32 size;
  bool loaded;

public:
  // TODO: add mesh data as first argument
  Model(const uint8_t* data, u32 size);
  Model(const char* filename);
  ~Model();
  void render();

  inline bool is_loaded() const { return loaded; }
};

#endif // MODEL_H