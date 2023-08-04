#include "stage.h"

#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "stage_obj.h"
#include "fx_obj.h"

std::unique_ptr<Model> Stage::stagemodel = nullptr;
std::unique_ptr<Model> Stage::fxmodel = nullptr;

Stage::Stage() {
  if (stagemodel == nullptr)
    stagemodel = std::make_unique<Model>(stage_obj, stage_obj_size);
  
  if (fxmodel == nullptr)
    fxmodel = std::make_unique<Model>(fx_obj, fx_obj_size);

  // Set texmtx to the bottom left of the texture
  guMtxIdentity(texMtx);
  guMtxTrans(texMtx, 0.0f, 0.5f, 0.0f);
  guMtxScale(texMtx, 0.5f, 0.5f, 0);
}

void Stage::update(f32 deltatime) {
  GameObject::update(deltatime);

  fxtransform.rotate(0.0f, 0.0f, 22.5f * deltatime);
  fxtransform.update();
}

void Stage::render() {
  GFX_Texture(TEX_MODEL, texMtx);
  GFX_EnableLighting(true);
  GFX_EnableColor(true);

  GFX_EnableAlphaTest(true);
  GFX_SetWriteBuffers(true, true, true);

  GFX_ModelViewMatrix(transform->matrix);
  GFX_SetBlendMode(MODE_OFF);
  stagemodel->render();

  GFX_ModelViewMatrix(fxtransform.matrix);
  GFX_SetBlendMode(MODE_BLEND);
  fxmodel->render();
}
