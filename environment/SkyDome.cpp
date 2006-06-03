// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

#include <sage/sage.h>
#include <sage/GL.h>
#include <math.h>
#include <iostream>
#include "renderers/Render.h"
#include "src/System.h"
#include "renderers/Graphics.h"
#include "src/Calendar.h"

#include "renderers/RenderSystem.h"

#define SQR(X) ((X))

#include "SkyDome.h"
                                                                          
#define VERTEX_COORDS { \
  -1.0f,  1.0f,  1.0f,  \
  -1.0f, -1.0f,  1.0f,  \
   1.0f, -1.0f,  1.0f,  \
   1.0f,  1.0f,  1.0f,  \
                        \
  -1.0f,  1.0f, -1.0f,  \
  -1.0f,  1.0f,  1.0f,  \
   1.0f,  1.0f,  1.0f,  \
   1.0f,  1.0f, -1.0f,  \
                        \
   1.0f, -1.0f, -1.0f,  \
   1.0f, -1.0f,  1.0f,  \
  -1.0f, -1.0f,  1.0f,  \
  -1.0f, -1.0f, -1.0f,  \
                        \
   1.0f,  1.0f, -1.0f,  \
   1.0f,  1.0f,  1.0f,  \
   1.0f, -1.0f,  1.0f,  \
   1.0f, -1.0f, -1.0f,  \
                        \
  -1.0f ,-1.0f, -1.0f,  \
  -1.0f, -1.0f,  1.0f,  \
  -1.0f,  1.0f,  1.0f,  \
  -1.0f,  1.0f, -1.0f,  \
                        \
  -1.0f, -1.0f, -1.0f,  \
  -1.0f,  1.0f, -1.0f,  \
   1.0f,  1.0f, -1.0f,  \
   1.0f, -1.0f, -1.0f }

#define DIST 100.0f

#define QUAD_COORDS { \
	-DIST,  DIST, 1.0f, \
	 DIST,  DIST, 1.0f, \
	 DIST, -DIST, 1.0f, \
	-DIST, -DIST, 1.0f}

#define QUAD_TEX { \
	0, 0, \
	5, 0, \
	5, 5, \
	0, 5}

namespace Sear {

float SkyDome::m_box[] = VERTEX_COORDS;
float SkyDome::m_quad_v[] = QUAD_COORDS;
float SkyDome::m_quad_t[] = QUAD_TEX;

SkyDome::SkyDome(float radius, int levels, int segments) :
    m_verts(NULL), m_texCoords(NULL),
    m_vb_verts(0), m_vb_texCoords(0),
    m_radius(radius), m_levels(levels), m_segments(segments),
    m_context_no(-1)
{
    m_size = segments * (levels + 1); // extra level for the skirt
    domeInit(radius, levels, segments);
}

SkyDome::~SkyDome()
{
    delete [] m_verts;
    delete [] m_texCoords;
}

float* SkyDome::genLevelVerts(float a2, float a22, float radius, int segments, float* verts)
{
    float segmentInc = (2.0f * M_PI) / segments;
    
    for (int j = 0; j < segments; ++j) {
      float a1 = ((float)j * segmentInc);
      float a11 = ((float)(j + 1) * segmentInc);

      float x,y,z;
      
      x = sin(a1) * sin(a2) * SQR(radius);
      y = cos(a1) * sin(a2) * SQR(radius);
      z = cos(a2) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
      
      x = sin(a11) * sin(a2) * SQR(radius);
      y = cos(a11) * sin(a2) * SQR(radius);
      z = cos(a2) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
      
      x = sin(a11) * sin(a22) * SQR(radius);
      y = cos(a11) * sin(a22) * SQR(radius);
      z = cos(a22) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
      
      x = sin(a1) * sin(a22) * SQR(radius);
      y = cos(a1) * sin(a22) * SQR(radius);
      z = cos(a22) * radius;

      (*verts++) = x;
      (*verts++) = y;
      (*verts++) = z;
    }
    
    return verts;
}

float *SkyDome::genVerts(float radius, int levels, int segments)
{
  float *verts = new float[m_size * 3 * 4];
  float levelInc = M_PI / (float)levels / 2.0f;
    
  float* vptr = genLevelVerts((M_PI * 3) / 4, M_PI / 2, radius, segments, verts);

  for (int i = 0; i < levels; ++i) {
    float a2 = ((float)i * levelInc);
    float a22 = ((float)(i + 1) * levelInc);
    vptr = genLevelVerts(a2, a22, radius, segments, vptr);
  }
  
  return verts;
}
 
float *SkyDome::genTexCoords(float radius, int levels, int segments)
{
  float *tex = new float[m_size * 2 * 4];
  int tex_counter = -1;

 // tex coords for the skirt
  for (int k = 0; k < (segments * 8); ++k) {
    tex[++tex_counter] = 0.0f;
  }

  for (int i = 0; i < levels; ++i) {

    float v1 = (float)(levels - i)/(float)(levels);
    float v2 = (float)(levels - (i + 1))/(float)(levels);
    for (int j = 0; j < segments; ++j) {
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v1;
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v1;
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v2;
      tex[++tex_counter] = 0.0f;
      tex[++tex_counter] = v2;
    }
  }
  return tex;
}

void SkyDome::domeInit(float radius, int levels, int segments) {
  // Get texture handles
  m_textures[0] = RenderSystem::getInstance().requestTexture("atmosphere");
  m_textures[1] = RenderSystem::getInstance().requestTexture("cloud_layer_1");
  m_textures[2] = RenderSystem::getInstance().requestTexture("cloud_layer_2");

  if (m_verts) delete [] m_verts;
  if (m_texCoords) delete [] m_texCoords;

  m_verts = genVerts(radius, levels, segments);  
  m_texCoords = genTexCoords(radius, levels, segments);  

  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glGenBuffersARB(1, &m_vb_verts);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_verts);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_size * 3 * 4 * sizeof(float), m_verts, GL_STATIC_DRAW_ARB);
    delete [] m_verts;
    m_verts = NULL;

    glGenBuffersARB(1, &m_vb_texCoords);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texCoords);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_size * 2 * 4 * sizeof(float), m_texCoords, GL_STATIC_DRAW_ARB);
    delete [] m_texCoords;
    m_texCoords = NULL;
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void SkyDome::contextDestroyed(bool check) {

  if (m_context_no == -1) return;

  if (check) {
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      if (glIsBufferARB(m_vb_verts)) {
        glDeleteBuffersARB(1, &m_vb_verts);
      }

      if (glIsBufferARB(m_vb_texCoords)) {
        glDeleteBuffersARB(1, &m_vb_texCoords);
      }
    }
  }
  m_vb_verts = 0;
  m_vb_texCoords = 0;

  m_context_no = -1;
}

void SkyDome::contextCreated() {
  assert(RenderSystem::getInstance().getRenderer()->contextValid());

  assert(m_context_no == -1);
  m_context_no = RenderSystem::getInstance().getRenderer()->currentContextNo();
  domeInit(m_radius, m_levels, m_segments);
}

void SkyDome::getHorizonColors()
{
    m_horizonColors.clear();
    
    RenderSystem::getInstance().switchTexture(m_textures[0]); // atmosphere tex
    GLint width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    
    GLubyte* skyTexels = new GLubyte[width * height * 4];

//    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, skyTexels);
    int glerr = glGetError();
    if (glerr != GL_NO_ERROR) {
        std::cerr << "got error while trying to read atmosphere texels" << std::endl;
        return;
    }

    GLubyte* texel = skyTexels;
    for (int S=0; S < width; ++S) {
        Color_4 c;
        c.r = *texel++;
        c.g = *texel++;
        c.b = *texel++;
        c.a = *texel++;
        m_horizonColors.push_back(c);
    }

    delete[] skyTexels;
}

void SkyDome::updateFogColor(float t)
{
    if (m_horizonColors.empty()) getHorizonColors();

    assert (m_horizonColors.empty() == false);

    // ensure t is in the range [0.0 .. 1.0)
    if (t < 0.0f) t = 0.0f;
    if (t >= 1.0f) t = 0.0f;
       
    t *= m_horizonColors.size(); // t is now in [0.0 ... num_colors)
    
    // offset so integral values map to half way between two pixels
    t -= 0.5f;
    if (t < 0.0) t+= m_horizonColors.size();
    
// compute I and J indices into horizon colors, then interpolate
// note this code uses lots of local variables, for clarity, and we only
// run this once per frame
    unsigned int I = static_cast<unsigned int>(t); // truncation is desired here
    unsigned int J = (I == (m_horizonColors.size() - 1)? 0 : I + 1);
    
    Color_4 lower = m_horizonColors[I],
        upper = m_horizonColors[J];
    
    const float interp = t - I, // get the fractional part of t
        invInterp = 1.0f - interp;
                
    float color[4];
    color[0] = ((lower.r * invInterp) + (upper.r * interp)) / 255.0f;
    color[1] = ((lower.g * invInterp) + (upper.g * interp)) / 255.0f;
    color[2] = ((lower.b * invInterp) + (upper.b * interp)) / 255.0f;
    color[3] = 1.0f;
    glFogfv(GL_FOG_COLOR, color);
}

void SkyDome::render() {

  assert(RenderSystem::getInstance().getRenderer()->contextValid());
  assert(m_context_no == RenderSystem::getInstance().getRenderer()->currentContextNo());

  glColor3f(1.0f, 1.0f, 1.0f);
  Calendar *cal = System::instance()->getCalendar();
  assert (cal != NULL);
  float val = cal->getHours();
  val *= (float)cal->getMinutesPerHour();
  val += (float)cal->getMinutes();
  val *= (float)cal->getSecondsPerMinute();
  val += (float)cal->getSeconds();

  val /= (float)(cal->getSecondsPerMinute() * cal->getMinutesPerHour() * cal->getHoursPerDay());

  updateFogColor(val);
  
 
  // Select atmosphere texture
  RenderSystem::getInstance().switchTexture(m_textures[0]);

  // Translate texture for time of day 
  // and simple cloud animation
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
    glLoadIdentity();
    glTranslatef(val, 0.0f,0.0f);
  glMatrixMode(GL_MODELVIEW);

  // Set the dome vetices
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_verts);
  }
  glVertexPointer(3, GL_FLOAT, 0, m_verts);

  glEnable(GL_BLEND);

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  // Setup dome texture coords
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texCoords);
  }
  glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
  // Renderdome
  glDrawArrays(GL_QUADS, 0, m_size * 4);

  glDisable(GL_BLEND);

  // Render Cloud layer one
  glEnable(GL_BLEND);
  RenderSystem::getInstance().switchTexture(m_textures[1]);
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }

  // Render large polygon for cloud layer
  // TODO split into smaller polys
  // turn down edges so its not so flat looking
//  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
//    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
//  }
  
  glVertexPointer(3, GL_FLOAT, 0, &m_quad_v[0]);
  glTexCoordPointer(2, GL_FLOAT, 0, &m_quad_t[0]);
  glDrawArrays(GL_QUADS, 0, 4);

  // Translate further so cloud layers move
  // at different speeds
  glMatrixMode(GL_TEXTURE);
    glTranslatef(val, 0.0f,0.0f);
  glMatrixMode(GL_MODELVIEW);

  // Render cloud layer two
  RenderSystem::getInstance().switchTexture(m_textures[2]);

  glVertexPointer(3, GL_FLOAT, 0, &m_quad_v[0]);
  glTexCoordPointer(2, GL_FLOAT, 0, &m_quad_t[0]);
  glDrawArrays(GL_QUADS, 0, 4);

  // reset texture matrix
  glMatrixMode(GL_TEXTURE);
    glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  // Reset states
  glDisable(GL_BLEND);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  // Disable vertex buffer objects
//  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { 
//    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
//  }
}

} /* namespace Sear */
