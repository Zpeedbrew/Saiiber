#include "block.h"

#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "cube_obj.h"

std::unique_ptr<Model> Block::model = nullptr;

Block::Block() {
  if (model == nullptr)
    model =
        std::make_unique<Model>(cube_obj, cube_obj_size);
  
  // Set texmtx to the top left of the texture
  guMtxIdentity(texMtx);
  guMtxTransApply(texMtx, texMtx, 0.0f, 0.5f, 0.0f);
  guMtxScaleApply(texMtx, texMtx, 0.5f, 0.5f, 0);
}

void Block::update(f32 deltatime) {
  GameObject::update(deltatime);
}

void Block::render() {
  GFX_BindTexture(TEX_MODEL);
  GFX_EnableTexture(true);
  GFX_EnableLighting(true);

  GFX_ModelViewMatrix(transform->matrix);
  GFX_TextureMatrix(true, texMtx);

  GFX_EnableAlphaTest(false);
  GFX_SetBlendMode(MODE_SOURCE);
  GFX_SetWriteBuffers(true, true, true);

  model->render();
}
