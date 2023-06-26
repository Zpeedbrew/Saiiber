#include "model.h"
#include <malloc.h>
#include <gccore.h>
#include <fstream>
#include <limits.h>
#include <cstring>

#define ASSET_PATH "sd:/apps/saiiber/assets"
#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "../gfx.h"
#include "../logger.h"

Model::Model(const char* filename) {
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s%s", ASSET_PATH, filename);
  LOG_DEBUG("Loading model %s\n", path);
 
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  std::ifstream file(path, std::ios::binary);
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &file)) {
    file.close();
    LOG_ERROR("Failed to load model %s: %s\n", path, err);
    return;
  }

  file.close();

  size_t indices = 0;
  for (const auto& shape : shapes) {
    LOG_DEBUG("Shape %s has %d indices and %d vertices\n", shape.name.c_str(), shape.mesh.indices.size(), shape.mesh.num_face_vertices.size());
    indices += shape.mesh.indices.size();
  }

  LOG_DEBUG("Initially loaded %s with:\n\tIndices: %d\n\tVertices: %d\n\tTexCoords: %d\n",
    path, indices, attrib.vertices.size(), attrib.texcoords.size());

  size_t szVertices = indices * 3 * sizeof(s16);
  size_t szTexCoords = indices * 2 * sizeof(u16);
  size_t szColor = indices * 4 * sizeof(u8);
  size_t total = 3 + szVertices + szTexCoords + szColor + 63;

  if (total % 64 > 0)
    total += 64 - (total % 64);

  LOG_DEBUG("Initial size: %d\n", total);

  display = memalign(32, total);
  memset(display, 0, total);
  DCInvalidateRange(display, total);
  GX_BeginDispList(display, total);

  // Models in obj format must be normalized and triangulated
  // Normalized so the vertices will be between -1 and 1
  // That way they can be converted to s16
  {
    for (const auto& shape : shapes) {
      GX_Begin(GX_TRIANGLES, GX_VTXFMT0, shape.mesh.indices.size());
      for (const auto& index : shape.mesh.indices) {
        GX_Position3s16(attrib.vertices[3 * index.vertex_index + 0] * 32768,
                        attrib.vertices[3 * index.vertex_index + 1] * 32768,
                        attrib.vertices[3 * index.vertex_index + 2] * 32768);

        GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);

        GX_TexCoord2u16(attrib.texcoords[2 * index.texcoord_index + 0] * 65536,
                        attrib.texcoords[2 * index.texcoord_index + 1] * 65536);
      }
      GX_End();
    }
  }
  size = GX_EndDispList();
  if (size == 0)
    LOG_ERROR("Failed to create display list for model\n");
  LOG_DEBUG("Final size: %d\n", size);
}

Model::~Model() {
  free(display);
}

void Model::render() {
  GX_CallDispList(display, size);
}