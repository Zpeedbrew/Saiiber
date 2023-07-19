#include "saber.h"

#include "../gfx.h"
#include "../resource/model.h"
#include "saber_obj.h"

std::unique_ptr<Model> Saber::model = nullptr;

Saber::Saber(std::shared_ptr<Transform> transform) : GameObject(transform) {
  if (model == nullptr)
    model = std::make_unique<Model>(saber_obj, saber_obj_size);

  // Set texmtx to the top right of the texture
  guMtxIdentity(texMtx);
  guMtxTransApply(texMtx, texMtx, 256, 256, 0);  // could be 256 or could be 0.5
  guMtxScaleApply(texMtx, texMtx, 0.5f, 0.5f, 0);
}

// Transform doesn't need updated because we already update it in the main loop
void Saber::update(f32 deltatime) { GameObject::update(deltatime); }

void Saber::render() {
  GFX_BindTexture(TEX_MODEL);
  GFX_TextureMatrix(true, texMtx);
  GFX_EnableAlphaTest(true);
  GFX_SetBlendMode(MODE_BLEND);
  GFX_SetWriteBuffers(true, true, true);

  GFX_ModelViewMatrix(transform->matrix);
  model->render();
}