#include "loading_scene.h"
#include <ogc/gx.h>
#include "../gfx.h"
#include "../logger.h"
template <typename T>
LoadingScene<T>::LoadingScene(T*nextScene) {
this->nextScene=nextScene;
LOG_DEBUG("Loading scene constructed.Loading resources.\n");
}
template<typename T>
void LoadingScene<T>::init(){
GFX_SetBlendMode(MODE_BLEND);
GFX_EnableAlphaTest(false);
GFX_SetWriteBuffers(true, false, false);
GFX_Texture(TEX_GUI);
GFX_EnableLighting(false);
glm::mat4 ortho=
      glm::ortho(0.0f, (f32)SCREEN_WIDTH, (f32)SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
  GFX_Projection(ortho, ORTHOGRAPHIC);
  view = glm::mat4(1.0f);
  GFX_ModelMatrix(view);
}

template <typename T>
void LoadingScene<T>::update(f32 deltatime) {
if(nextScene->isLoaded()) {
// We're done loading, so we can switch to the next scene
ChangeScene<T>(nextScene);
}
}
template <typename T>
void LoadingScene<T>::render() {
// GX_VTXFMT1 is where our GUI vertices are stored
GX_Begin(GX_QUADS, GX_VTXFMT1, 4);
// Top left
GX_Position3s16(0, 0, 0);
GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
GX_TexCoord2u16(0, 0);
// top right
GX_Position3s16(640, 0, 0);
GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
GX_TexCoord2u16(256, 0);
// bottom right
GX_Position3s16(640, 480, 0);
GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
GX_TexCoord2u16(256, 256);
//bottom left
GX_Position3s16(0, 480, 0);
GX_Color4u8(0xFF,0xFF,0xFF,0xFF);
GX_TexCoord2u16(0,256);
GX_End();
}