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
struct ModelImpl {
tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t>shapes;
std::vector<tinyobj::material_t>materials;
u32 size;
};
void* make_model(ModelImpl& impl){
size_t indices=0;
for(const auto&shape:impl.shapes){
LOG_DEBUG("Shape %s has %d indices and %d vertices\n",hape.name.c_str(),shape.mesh.indices.size(), shape.mesh.num_face_vertices.size());
indices+=shape.mesh.indices.size();
}
LOG_DEBUG(
"Initially loaded model with:\n\tIndices: %d\n\tVertices: "
"%d\n\tTexCoords: %d\n",indices,impl.attrib.vertices.size(),impl.attrib.texcoords.size());
size_t szVertices=indices*3*sizeof(s16);
size_t szTexCoords=indices*2*sizeof(u16);
size_t szColor=indices*4*sizeof(u8);
size_t total=3+szVertices+szTexCoords+szColor+63;
#ifdef SHADERS
size_t szNormals=indices*3*sizeof(s16);
total+=szNormals;
#endif
if(total%64>0)total+=64-(total%64);
LOG_DEBUG("Initial size: %d\n",total);
void* display=memalign(32,total);
memset(display, 0, total);
DCInvalidateRange(display,total);
GX_BeginDispList(display,total);
// Models in obj format must be normalized and triangulated
// Normalized so the vertices will be between -1 and 1
// That way they can be converted to s16
{
for (const auto&shape:impl.shapes) {
GX_Begin(GX_TRIANGLES,MODELFMT,shape.mesh.indices.size());
for (const auto&index:shape.mesh.indices){
int x=(int)impl.attrib.vertices[3*index.vertex_index+0];
int y=(int)impl.attrib.vertices[3*index.vertex_index+1];
int z=(int)impl.attrib.vertices[3*index.vertex_index+2];
int xfrac=(impl.attrib.vertices[3*index.vertex_index+0]-x)*256.0f;
int yfrac=(impl.attrib.vertices[3*index.vertex_index+1]-y)*256.0f;
int zfrac=(impl.attrib.vertices[3*index.vertex_index+2]-z)*256.0f;
x=x<<8;
y=y<<8;
z=z<<8;
x+=xfrac;
y+=yfrac;
z+=zfrac;
GX_Position3s16(x,y,z);
#ifdef SHADERS
GX_Normal3s16(impl.attrib.normals[3*index.normal_index+0]*0x7FFF,impl.attrib.normals[3*index.normal_index+1]*0x7FFF,impl.attrib.normals[3*index.normal_index+2]*0x7FFF);
#endif

        GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);

        GX_TexCoord2u16(
            impl.attrib.texcoords[2 * index.texcoord_index + 0] * 0xFFFF,
            impl.attrib.texcoords[2 * index.texcoord_index + 1] * 0xFFFF);
      }
      GX_End();
    }
  }

  impl.size = GX_EndDispList();
  if (impl.size == 0) {
    LOG_ERROR("Failed to create display list for model\n");
    free(display);
    return NULL;
  }

  LOG_DEBUG("Final Size: %d\n", impl.size);
  return display;
}

Model::Model(const char* filename) {
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s%s", ASSET_PATH, filename);
  LOG_DEBUG("Loading Model %s\n", path);

  ModelImpl impl;
  std::ifstream file(path, std::ios::binary);
  if (!tinyobj::LoadObj(&impl.attrib, &impl.shapes, &impl.materials, &err,
                        &file)) {
    display = make_model(impl);
    loaded = display != NULL;
    this->size = impl.size;
  }

  file.close();
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
Model::Model(const uint8_tdata,u32 size) {
membufbuf((char*)data,size);
std::istream stream(&buf);
ModelImpl impl;
impl.size=size;
if (!tinyobj::LoadObj(&impl.attrib,&impl.shapes,&impl.materials,&err,&stream))
return;
display=make_model(impl);
loaded=display != NULL;
this->size=impl.size;
}
Model::~Model(){free(display);}
void Model::render(){GX_CallDispList(display,size);}