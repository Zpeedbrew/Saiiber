#include "saber.h"

#include "../gfx.h"
#include "../resource/model.h"
#include "saber_obj.h"

std::unique_ptr<Model> Saber::model =
    std::make_unique<Model>(saber_obj, saber_obj_size);

// Transform doesn't need updated because we already update it in the main loop
void Saber::update(f32 deltatime) { }

void Saber::render() {
  GFX_BindTexture(TEX_MODEL);
  GFX_EnableAlphaTest(true);
  model->render();
}