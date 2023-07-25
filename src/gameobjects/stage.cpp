#include "stage.h"

#include "../gfx.h"
#include "../resource/model.h"
#include "stage_obj.h"

Stage::Stage() {
  if (model == nullptr)
    model = std::make_unique<Model>(stage_obj, stage_obj_size);

  // Set texmtx to the bottom left of the texture
  guMtxIdentity(texMtx);
  guMtxTransApply(texMtx, texMtx, 0.0f, 0.5f, 0.0f);
  guMtxScaleApply(texMtx, texMtx, 0.5f, 0.5f, 0);
}

void Stage::update(f32 deltatime) { GameObject::update(deltatime); }

void Stage::render() {
  GFX_BindTexture(TEX_MODEL);
  GFX_EnableTexture(true);
  GFX_EnableLighting(true);
  GFX_EnableColor(true);

  GFX_ModelViewMatrix(transform->matrix);
  GFX_TextureMatrix(true, texMtx);

  GFX_EnableAlphaTest(false);
  GFX_SetBlendMode(MODE_OFF);
  GFX_SetWriteBuffers(true, true, true);
  model->render();
}
