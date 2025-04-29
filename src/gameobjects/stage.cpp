#include "stage.h"
#include "../gfx.h"
#include "../logger.h"
#include "../resource/model.h"
#include "fx_obj.h"
#include "stage_obj.h"
std::unique_ptr<Model>Stage::stagemodel=nullptr;
std::unique_ptr<Model>Stage::fxmodel=nullptr;
Stage::Stage() {
if(stagemodel==nullptr)
stagemodel=std::make_unique<Model>(stage_obj,stage_obj_size);
texMtx=glm::scale(glm::vec3(0.5f,0.5f,0.5f));
//if(fxmodel==nullptr)
//xmodel=std::make_unique<Model>(fx_obj,fx_obj_size);
//Set texmtx to the bottom left of the texture
/*
guMtxIdentity(texMtx);
guMtxScale(texMtx, 0.5f, 0.5f, 0);  
guMtxTrans(texMtx, 0.5f, 0.0f, 0.0f);
*/
LOG_DEBUG("Stage Matrix:\n");
GFX_OutputMatrix(texMtx);
}
void Stage::update(f32 deltatime) {
GameObject::update(deltatime);
//fxtransform.rotate(0.0f,0.0f,22.5f*deltatime);
//fxtransform.update();
}
void Stage::render() {
GFX_Texture(TEX_MODEL,texMtx);
GFX_EnableLighting(true);
GFX_EnableColor(true);
GFX_EnableAlphaTest(true);
GFX_SetWriteBuffers(true,true,true);
GFX_ModelMatrix(transform->matrix);
GFX_SetBlendMode(MODE_OFF);
stagemodel->render();
//GFX_ModelViewMatrix(fxtransform.matrix);
//GFX_SetBlendMode(MODE_BLEND);
//fxmodel->render();
}