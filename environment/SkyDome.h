
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
  SkyDome(float radius, int level, int segments);
  virtual ~SkyDome();
  
  void render();
  void invalidate() {}
    
private:
    float *genVerts(float radius, int levels, int segments);
  float *genTexCoords(float radius, int levels, int segments);

  void domeInit(float radius, int levels, int segments);
  
  float *m_verts, *m_texCoords;
  int m_size;
  
  unsigned int m_vb_verts, m_vb_texCoords;
  static float m_box[];
  static float m_quad_v[];
  static float m_quad_t[];

  TextureID m_textures[5];
};

} // namespace Sear

#endif
