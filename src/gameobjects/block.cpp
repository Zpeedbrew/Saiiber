#include "block.h"

#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "cube_obj.h"

std::unique_ptr<Model> Block::model =
    std::make_unique<Model>(cube_obj, cube_obj_size);

void Block::update(f32 deltatime) {}

void Block::render() {
  GFX_BindTexture(TEX_MODEL);
  GFX_TextureMatrix(true, texMtx);
  GFX_EnableAlphaTest(true);
  model->render();
}

void Block::setTexMtx(s16 x, s16 y, s16 w, s16 h) {
  guMtxIdentity(texMtx);
  guMtxTransApply(texMtx, texMtx, x / 160.0f, y / 32.0f, 0);
  guMtxScaleApply(texMtx, texMtx, w / 160.0f, h / 32.0f, 0);
}
