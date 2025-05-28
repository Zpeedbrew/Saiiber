#ifndef FX_H
#define FX_H
#include "gameobject.h"
class Model;
class FX:public GameObject {
private:
static std::unique_ptr<Model> model;
glm::mat4 texMtx;
public:
FX();
void update(f32 deltatime);
void render();
};
#endif // BLOCK_H
