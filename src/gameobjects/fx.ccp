#include "block.h"
#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "cube_obj.h"
std::unique_ptr<Model> Block::model = nullptr;
Block::Block() {
  if(model == nullptr)
  model=std::make_unique<Model>(cube_obj, cube_obj_size);
  texMtx=glm::translate(glm::vec3(0.0f,0.5f,0.0f))*glm::scale(glm::vec3(0.5f,0.5f,0.5f));
  LOG_DEBUG("Block Matrix\n");
  GFX_OutputMatrix(texMtx);
}
void Block::update(f32 deltatime) { GameObject::update(deltatime); }
void Block::render() {
  GFX_Texture(TEX_MODEL, texMtx);
  // This was important for the
  GFX_EnableColor(true);
  GFX_EnableLighting(true);
  // Breaking here
  GFX_ModelMatrix(transform->matrix);
  GFX_EnableAlphaTest(false);
  GFX_SetBlendMode(MODE_OFF);
  GFX_SetWriteBuffers(true,true,true);
  // GX_SetTevOp(GX_TEVSTAGE0, GX_DECAL);
  model->render();
