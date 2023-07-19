#include "model.h"

#include <gccore.h>
#include <limits.h>
#include <malloc.h>

#include <cstring>

#define ASSET_PATH "sd:/apps/saiiber/assets"
#define TINYOBJLOADER_IMPLEMENTATION

#include "../gfx.h"
#include "../logger.h"
#include "tiny_obj_loader.h"

tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;
std::string err;

void* make_model(u32& size) {
  size_t indices = 0;
  for (const auto& shape : shapes) {
    LOG_DEBUG("Shape %s has %d indices and %d vertices\n", shape.name.c_str(),
              shape.mesh.indices.size(), shape.mesh.num_face_vertices.size());
    indices += shape.mesh.indices.size();
  }

  LOG_DEBUG(
      "Initially loaded model with:\n\tIndices: %d\n\tVertices: "
      "%d\n\tTexCoords: %d\n",
      indices, attrib.vertices.size(), attrib.texcoords.size());

  size_t szVertices = indices * 3 * sizeof(s16);
  size_t szTexCoords = indices * 2 * sizeof(u16);
  size_t szNormals = indices * 3 * sizeof(s16);
  size_t szColor = indices * 4 * sizeof(u8);
  size_t total = 3 + szVertices + szTexCoords + szColor + szNormals + 63;

  if (total % 64 > 0) total += 64 - (total % 64);

  LOG_DEBUG("Initial size: %d\n", total);

  void* display = memalign(32, total);
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

        GX_Normal3s16(attrib.normals[3 * index.normal_index + 0] * 32768,
                      attrib.normals[3 * index.normal_index + 1] * 32768,
                      attrib.normals[3 * index.normal_index + 2] * 32768);

        GX_TexCoord2u16(attrib.texcoords[2 * index.texcoord_index + 0] * 65536,
                        attrib.texcoords[2 * index.texcoord_index + 1] * 65536);
      }
      GX_End();
    }
  }

  size = GX_EndDispList();
  if (size == 0) {
    LOG_ERROR("Failed to create display list for model\n");
    free(display);
    return NULL;
  }

  LOG_DEBUG("Final size: %d\n", size);
  return display;
}

Model::Model(const char* filename) {
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s%s", ASSET_PATH, filename);
  LOG_DEBUG("Loading model %s\n", path);

  std::ifstream file(path, std::ios::binary);
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &file)) {
    file.close();
    return;
  }

  file.close();

  display = make_model(this->size);
  loaded = display != NULL;
}

/*
  Trivial stream buffer
  For a small area of memory the difference may not matter.
  Although the saved allocation can be noticable there, too.
  For large chunks of memory, it makes a major difference.
*/
struct membuf : std::streambuf {
  membuf(char* base, std::ptrdiff_t n) { this->setg(base, base, base + n); }
};

Model::Model(const uint8_t* data, u32 size) {
  membuf buf((char*)data, size);
  std::istream stream(&buf);

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &stream)) return;

  display = make_model(this->size);
  loaded = display != NULL;
}

Model::~Model() { free(display); }

void Model::render() { GX_CallDispList(display, size); }