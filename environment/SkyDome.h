
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
    m_verts(NULL), m_texA(NULL), m_texB(NULL),
    m_vb_verts(0), m_vb_texA(0), m_vb_texB(0)
  {}
  virtual ~SkyDome() {}

  void init() {

  }

  void shutdown() {
    if (m_verts) {
      delete [] m_verts;
      m_verts = NULL;
    }
    if (m_texA) {
      delete [] m_texA;
      m_texA = NULL;
    }
    if (m_texB) {
      delete [] m_texB;
      m_texB = NULL;
    }
  }

  float *genVerts(float radius, int levels, int segments);
  float *genTexCoordsA(float radius, int levels, int segments);
  float *genTexCoordsB(float radius, int levels, int segments);

  void domeInit(float radius, int levels, int segments);
  void render(float radius, int levels, int segments);

private:
  bool m_initialised;
  float *m_verts, *m_texA, *m_texB;
  unsigned int m_vb_verts, m_vb_texA, m_vb_texB;

  TextureID m_textures[5];
};

}

#endif
