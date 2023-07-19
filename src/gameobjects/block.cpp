#include "block.h"

#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "cube_with_norms_obj.h"

std::unique_ptr<Model> Block::model = nullptr;

Block::Block() {
  if (model == nullptr)
    model =
        std::make_unique<Model>(cube_with_norms_obj, cube_with_norms_obj_size);
  
  // Set texmtx to the top left of the texture
  guMtxIdentity(texMtx);
  guMtxTransApply(texMtx, texMtx, 0, 0, 0);
  guMtxScaleApply(texMtx, texMtx, 0.5f, 0.5f, 0);
}

void Block::update(f32 deltatime) {
  GameObject::update(deltatime);
}

void Block::render() {
  GFX_ModelViewMatrix(transform->matrix);

  GFX_BindTexture(TEX_MODEL);
  GFX_TextureMatrix(true, texMtx);

  GFX_EnableAlphaTest(true);

  GFX_SetBlendMode(MODE_BLEND);
  GFX_SetWriteBuffers(true, true, true);

  model->render();
}
