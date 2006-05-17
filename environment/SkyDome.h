// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

#ifndef SEAR_SKYDOME_H
#define SEAR_SKYDOME_H 1

#include <sage/GL.h>

#include <math.h>
#include "renderers/RenderTypes.h"
#include "common/types.h"
#include <vector>

namespace Sear {

class SkyDome {
public:
  SkyDome(float radius, int level, int segments);
  virtual ~SkyDome();
  
  void render();
  void contextCreated();
  void contextDestroyed(bool check);
    
private:
    float* genVerts(float radius, int levels, int segments);
    float* genLevelVerts(float a2, float a22, float radius, int segments, float* verts);
    
  float *genTexCoords(float radius, int levels, int segments);

  void domeInit(float radius, int levels, int segments);
  
  void updateFogColor(float t);
  void getHorizonColors();
  
  float *m_verts, *m_texCoords;
  int m_size;
  
  GLuint m_vb_verts, m_vb_texCoords;
  static float m_box[];
  static float m_quad_v[];
  static float m_quad_t[];

  TextureID m_textures[5];
  
  std::vector<Color_4> m_horizonColors;

  float m_radius;
  int m_levels;
  int m_segments;

  int m_context_no;
};

} // namespace Sear

#endif
