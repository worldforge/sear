
#ifndef SEAR_SKYDOME_H
#define SEAR_SKYDOME_H 1

#include <sage/sage.h>
//#include <sage/GL.h>
#include <math.h>
#include <iostream>
#include "renderers/RenderSystem.h"


namespace Sear {

class SkyDome {
public:
  SkyDome() :
    m_initialised(false),
    m_verts(NULL), m_texCoords(NULL),
    m_vb_verts(0), m_vb_texCoords(0)
  {}

  virtual ~SkyDome();
  
void init() {

  }

  void shutdown() {
    if (m_verts) {
      delete [] m_verts;
      m_verts = NULL;
    }
    if (m_texCoords) {
      delete [] m_texCoords;
      m_texCoords = NULL;
    }
  }

  float *genVerts(float radius, int levels, int segments);
  float *genTexCoords(float radius, int levels, int segments);

  void domeInit(float radius, int levels, int segments);
  void render(float radius, int levels, int segments);

private:
  bool m_initialised;
  float *m_verts, *m_texCoords;
  unsigned int m_vb_verts, m_vb_texCoords;

  TextureID m_textures[5];
};

} // namespace Sear

#endif
