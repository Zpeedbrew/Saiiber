#include "debug_scene.h"

#include <ogc/gx.h>
#include <ogc/system.h>

#include "../gameobjects/camera.h"
#include "../gfx.h"
#include "../logger.h"
#include "../transform.h"

#define CUBEVERTEX(x, y, z)        \
  GX_Position3s16(x, y, z);        \
  GX_Normal3s16(0, 0, 0);          \
  GX_Color4u8(255, 127, 127, 255); \
  GX_TexCoord2s16(0, 0)

void GFX_OutputMatrix(Mtx44& matrix) {
  LOG_DEBUG("%f %f %f %f\n", matrix[0][0], matrix[0][1], matrix[0][2],
            matrix[0][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[1][0], matrix[1][1], matrix[1][2],
            matrix[1][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[2][0], matrix[2][1], matrix[2][2],
            matrix[2][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[3][0], matrix[3][1], matrix[3][2],
            matrix[3][3]);
}

void GFX_OutputMatrix(Mtx& matrix) {
  LOG_DEBUG("%f %f %f %f\n", matrix[0][0], matrix[0][1], matrix[0][2],
            matrix[0][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[1][0], matrix[1][1], matrix[1][2],
            matrix[1][3]);
  LOG_DEBUG("%f %f %f %f\n", matrix[2][0], matrix[2][1], matrix[2][2],
            matrix[2][3]);
}

guVector& operator*=(guVector& left, const guVector& right) {
  left.x *= right.x;
  left.y *= right.y;
  left.z *= right.z;
  return left;
}

guVector operator*(const float scalar, const guVector& v) {
  return {scalar * v.x, scalar * v.y, scalar * v.z};
}

guVector& operator*=(guVector& v, const float scalar) {
  v.x *= scalar;
  v.y *= scalar;
  v.z *= scalar;
  return v;
}

guVector operator+(const guVector& left, const guVector& right) {
  return {left.x + right.x, left.y + right.y, left.z + right.z};
}

guVector operator*(const guQuaternion& left, const guVector& right) {
  guVector xyz = {left.x, left.y, left.z};

  guVector t;
  guVecCross(&xyz, &right, &t);
  t *= 2.0f;

  guVector ct;
  guVecCross(&xyz, &t, &ct);

  return right + (left.w * t) + ct;
}

void outputVector(guVector vec) {
  LOG_DEBUG("%f %f %f\n", vec.x, vec.y, vec.z);
}

Mtx44 guProjection;
Mtx guModelView;

DebugScene::DebugScene() {}

void DebugScene::init() {
  GFX_SetBlendMode(MODE_BLEND);
  GFX_SetWriteBuffers(true, true, true);
  GFX_Texture(TEX_NONE);

  camera = std::make_shared<Camera>();
  modelTransform = std::make_shared<Transform>();

  // This is verifiably the correct perspective matrix.
  u64 start = SYS_Time();
  guPerspective(guProjection, 60.0f, (f32)SCREEN_WIDTH / (f32)SCREEN_HEIGHT,
                0.1f, 300.0f);
  LOG_DEBUG("guPerspective Matrix\n");
  GFX_OutputMatrix(guProjection);
  u64 end = SYS_Time();
  LOG_DEBUG("guPerspective took %llu ticks\n", end - start);

  start = SYS_Time();
  glm::mat4 transpose = glm::transpose(projection);
  LOG_DEBUG("glmPerspective matrix (transposed)\n");
  GFX_OutputMatrix(transpose);
  end = SYS_Time();
  LOG_DEBUG("glmPerspective took %llu ticks\n", end - start);

  // This is verifiably the correct view matrix (-Z forward)
  Mtx guView;
  guVector cam = {0.0F, 0.0F, 2.0F}, up = {0.0F, 1.0F, 0.0F},
           look = {0.0F, 0.0F, -1.0F};
  guLookAt(guView, &cam, &up, &look);

  LOG_DEBUG("guLookAt Matrix\n");
  GFX_OutputMatrix(guView);

  LOG_DEBUG("glmLookAt Matrix (Transposed)\n");
  transpose = glm::transpose(view);
  GFX_OutputMatrix(transpose);

  // This is verifiably the correct modelview matrix
  Mtx guModel;
  guVector axis = {1.0f, 0.0f, 0.0f};

  Mtx guTrans;
  guMtxIdentity(guTrans);
  guMtxTransApply(guTrans, guTrans, -1.5f, 0.0f, -6.0f);

  Mtx guRot;
  guMtxIdentity(guRot);
  guMtxRotDeg(guRot, 'x', 45.0f);

  Mtx guScale;
  guMtxScaleApply(guScale, guScale, 2.0f, 2.0f, 2.0f);

  // Trans Apply, Rotate Apply, Scale Apply!
  guMtxIdentity(guModel);
  guMtxConcat(guView, guModel, guModelView);

  LOG_DEBUG("guModel Matrix\n");
  GFX_OutputMatrix(guModel);
  LOG_DEBUG("guModelView Matrix\n");
  GFX_OutputMatrix(guModelView);

  modelTransform->position = glm::vec3(-1.5f, 0.0f, -6.0f);
  modelTransform->scale = glm::vec3(2.0f, 2.0f, 2.0f);
  modelTransform->rotate(45.0f, 0.0f, 0.0f);
  modelTransform->update();

  LOG_DEBUG("glmModel Matrix (Transposed)\n");
  transpose = glm::transpose(modelTransform->matrix);
  GFX_OutputMatrix(transpose);

  LOG_DEBUG("glmModelView Matrix (Transposed)\n");
  glm::mat4 modelview = glm::transpose(view * modelTransform->matrix);
  GFX_OutputMatrix(modelview);


  LOG_DEBUG("guRotation Matrix\n");
  GFX_OutputMatrix(guRot);

  LOG_DEBUG("glmRotation Matrix (Transposed)\n");
  transpose = glm::transpose(glm::toMat4(modelTransform->rotation));
  GFX_OutputMatrix(transpose);

  LOG_DEBUG("Loaded DebugScene\n");
}

void DebugScene::update(f32 deltatime) {
#ifdef _DEBUG
  camera->freecam(deltatime);
#endif
}

void renderCube() {
  GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 36);
  CUBEVERTEX(-0x100, -0x100, -0x100);
  CUBEVERTEX(-0x100, -0x100, 0x100);
  CUBEVERTEX(-0x100, 0x100, 0x100);

  CUBEVERTEX(0x100, 0x100, -0x100);
  CUBEVERTEX(-0x100, -0x100, -0x100);
  CUBEVERTEX(-0x100, 0x100, -0x100);

  CUBEVERTEX(0x100, -0x100, 0x100);
  CUBEVERTEX(-0x100, -0x100, -0x100);
  CUBEVERTEX(0x100, -0x100, -0x100);

  CUBEVERTEX(0x100, 0x100, -0x100);
  CUBEVERTEX(0x100, -0x100, -0x100);
  CUBEVERTEX(-0x100, -0x100, -0x100);

  CUBEVERTEX(-0x100, -0x100, -0x100);
  CUBEVERTEX(-0x100, 0x100, 0x100);
  CUBEVERTEX(-0x100, 0x100, -0x100);

  CUBEVERTEX(0x100, -0x100, 0x100);
  CUBEVERTEX(-0x100, -0x100, 0x100);
  CUBEVERTEX(-0x100, -0x100, -0x100);

  CUBEVERTEX(-0x100, 0x100, 0x100);
  CUBEVERTEX(-0x100, -0x100, 0x100);
  CUBEVERTEX(0x100, -0x100, 0x100);

  CUBEVERTEX(0x100, 0x100, 0x100);
  CUBEVERTEX(0x100, -0x100, -0x100);
  CUBEVERTEX(0x100, 0x100, -0x100);

  CUBEVERTEX(0x100, -0x100, -0x100);
  CUBEVERTEX(0x100, 0x100, 0x100);
  CUBEVERTEX(0x100, -0x100, 0x100);

  CUBEVERTEX(0x100, 0x100, 0x100);
  CUBEVERTEX(0x100, 0x100, -0x100);
  CUBEVERTEX(-0x100, 0x100, -0x100);

  CUBEVERTEX(0x100, 0x100, 0x100);
  CUBEVERTEX(-0x100, 0x100, -0x100);
  CUBEVERTEX(-0x100, 0x100, 0x100);

  CUBEVERTEX(0x100, 0x100, 0x100);
  CUBEVERTEX(-0x100, 0x100, 0x100);
  CUBEVERTEX(0x100, -0x100, 0x100);
  GX_End();
}

void DebugScene::render() {
  camera->render();

  GFX_Texture(TEX_NONE);
  GFX_SetBlendMode(MODE_OFF);

  // GFX_ModelMatrix(modelTransform->matrix);
  // renderCube();

  GFX_ModelMatrix(modelTransform->matrix);
  renderCube();
}
