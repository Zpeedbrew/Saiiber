#include "saber.h"
#include <malloc.h>
#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "saber_obj.h"
std::unique_ptr<Model>Saber::model=nullptr;
Saber::Saber(std::shared_ptr<Transform>transform)
    :GameObject(transform),color{255,0,0,127} {
  if(model==nullptr)
    model=std::make_unique<Model>(saber_obj,saber_obj_size);

  texMtx=glm::translate(glm::vec3(0.5f,0.5f,0.0f))*
           glm::scale(glm::vec3(0.5f,0.5f,0.5f));
  LOG_DEBUG("Saber Matrix:\n");
  GFX_OutputMatrix(texMtx);
}
void Saber::setColor(u8 r,u8 g,u8 b,u8 a) {color={r,g,b,a};}
// GameObject::update doesn't need to be called because this transform is shared with the remote
void Saber::update(f32 deltatime){
  // These are the +/- 0.09375 are the max/min values of the vertices
  // These are the furthest vertices from the center of the saber
  static glm::vec4 bottomleft=
      glm::vec4(-0.09375f,-0.09375f,1.0f,1.0f);
  static glm::vec4 topright=
      glm::vec4(0.09375f,0.845f,-1.0f,1.0f);

  // I have no idea what space this leads me to.
  glm::vec4 blSrc=projection*view*transform->matrix*bottomleft;
  glm::vec4 trSrc=projection*view*transform->matrix*topright;

  /* Translates to....
  tlSrc: X(-0.154318), Y(-1.449542), Z(-0.099175), W(2.573452)
  brSrc: X(0.154318), Y(-0.996877), Z(-0.099238), W(2.385952)

  W seems to correspond with the +/- X values relative to the screen
  I think it may be the scale?

  LOG_DEBUG("blSrc: X(%f), Y(%f), Z(%f), W(%f)\n", blSrc.x, blSrc.y, blSrc.z,
            blSrc.w);
  LOG_DEBUG("trSrc: X(%f), Y(%f), Z(%f), W(%f)\n", trSrc.x, trSrc.y, trSrc.z,
            trSrc.w);
  */

  // Transform to screen coordinates


  // These are screen coordinates (left, top, width, height)
  f32 left=(blSrc.x+1.0f)*(f32)SCREEN_WIDTH/2.0f;
  f32 bottom=(1.0f-blSrc.y)*(f32)SCREEN_HEIGHT/2.0f;
  f32 right=(trSrc.x+1.0f)*(f32)SCREEN_WIDTH/2.0f;
  f32 top=(1.0f-trSrc.y)*(f32)SCREEN_HEIGHT/2.0f;

  inView =
      left<SCREEN_WIDTH&&right>0&&bottom<SCREEN_HEIGHT&&top>0;

  // Round to nearest even number
  ppSource=glm::u16vec4(
      roundf(left/2.0f)*2,SCREEN_HEIGHT-(roundf(top/2.0f)*2),
      roundf(abs(right-left)/2)*2,roundf(abs(bottom-top)/2)*2);
}
void Saber::render() {
  // Calling these two will result in the TevOp being set to GX_REPLACE
  GFX_Texture(TEX_MODEL,texMtx);
  GFX_EnableLighting(false);

  // My presumption is blend is only ever "enabled" when TevOp is GX_BLEND
  GFX_SetBlendMode(MODE_OFF);
  GFX_EnableColor(true);
  GFX_EnableAlpha(true);
  GFX_ModelMatrix(transform->matrix);
  GFX_EnableAlphaTest(true);
  GFX_SetWriteBuffers(true,true,true);
  model->render();
}
u8 hasPrinted=0;
void Saber::postProcess(void*buf) {
if(!inView)return;
}