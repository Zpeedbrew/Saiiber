#include "loading_scene.h"
#include "../gfx.h"
#include "../logger.h"

LoadingScene::LoadingScene(Scene *nextScene) {
  this->nextScene = nextScene;
  LOG_DEBUG("Loading scene constructed. Loading resources.\n");
}

LoadingScene::~LoadingScene() { }

void LoadingScene::init() {
  GFX_EnableLighting(false);
  GFX_SetBlendMode(MODE_BLEND);
  GFX_EnableAlphaTest(false);
  GFX_TextureMatrix(false);
  GFX_SetWriteBuffers(true, false, false);

  GFX_BindTexture(TEX_GUI);

  Mtx44 ortho;
  guOrtho(ortho, 0, SCREEN_HEIGHT, 0, SCREEN_WIDTH, 0, 1);
  GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);

	guMtxIdentity(view);
  GFX_ModelViewMatrix(view);
}

void LoadingScene::update(f32 deltatime) {
  if (nextScene->isLoaded()) {
    // We're done loading, so we can switch to the next scene
    SetScene(nextScene);
  }
}

void LoadingScene::render() {
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

  // bottom left
  GX_Position3s16(0, 480, 0);
  GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
  GX_TexCoord2u16(0, 256);
  GX_End();
}
