// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 - 2003 Alistair Riddoch
// Copyright (C) 2004 - 2007 Simon Goodall

#include "TerrainRenderer.h"

#include "renderers/RenderSystem.h"

#include "renderers/Render.h"

#include <sage/sage.h>
#include <sage/GLU.h>
#include <sage/GL.h>

#include "src/System.h"
#include <Mercator/Segment.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/AreaShader.h>
#include <Mercator/Area.h>
#include <Mercator/Surface.h>

#include <iostream>

namespace Sear {

static const bool debug_flag = false;
static const int segSize = 64;

static GLfloat sx0[] = { 0.125f, 0.f, 0.f, 0.f };
static GLfloat ty0[] = { 0.f, 0.125f, 0.f, 0.f };

static GLfloat sx1[] = { 0.015625f, 0.f, 0.f, 0.f };
static GLfloat ty1[] = { 0.f, 0.015625f, 0.f, 0.f };


void TerrainRenderer::DataSeg::contextCreated() {
  assert(m_context_no == -1);
  m_context_no = RenderSystem::getInstance().currentContextNo();
}

void TerrainRenderer::DataSeg::contextDestroyed(bool check) {

  if (m_context_no == -1) return;

  if (check) {
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      if (glIsBufferARB(vb_narray)) {
        glDeleteBuffersARB(1, &vb_narray);
      }

      if (glIsBufferARB(vb_harray)) {
        glDeleteBuffersARB(1, &vb_harray);
      }     
    }
    if (glIsList(disp)) {
      glDeleteLists(1, disp);
    }
  }
  vb_narray = 0;
  vb_harray = 0;
  disp = 0;

  // Clean up buffers
  if (harray) delete [] harray;
  harray = NULL;
  narray = NULL;

  // Clean up textures
  std::map<int, GLuint>::iterator it = m_alphaTextures.begin();
  for (; it != m_alphaTextures.end(); ++it) {
      if (check && glIsTexture(it->second)) glDeleteTextures(1, &it->second);
  }
  
  m_alphaTextures.clear();

  m_context_no = -1;
}

void TerrainRenderer::enableRendererState() {
  static const float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
  static const float diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  static const float specular[] = { 0.f, 0.f, 0.f, 1.0f };
  static const float emission[] = { 0.f, 0.f, 0.f, 1.0f };
  static const float shininess[] = { 0.1f };
  glColor4f (1.f, 1.f, 1.f, 1.f);
  glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv (GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv (GL_FRONT, GL_EMISSION, emission);
  glMaterialfv (GL_FRONT, GL_SHININESS, shininess);
  glEnableClientState (GL_NORMAL_ARRAY);

  // Setting for texture Unit 1
  glActiveTexture (GL_TEXTURE1);
  glEnable (GL_TEXTURE_GEN_S);
  glEnable (GL_TEXTURE_GEN_T);
  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGenfv (GL_S, GL_OBJECT_PLANE, sx1);
  glTexGenfv (GL_T, GL_OBJECT_PLANE, ty1);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Settings for Texture Unit 0
  glActiveTexture (GL_TEXTURE0);
  glEnable (GL_TEXTURE_GEN_S);
  glEnable (GL_TEXTURE_GEN_T);
  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGenfv (GL_S, GL_OBJECT_PLANE, sx0);
  glTexGenfv (GL_T, GL_OBJECT_PLANE, ty0);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void TerrainRenderer::disableRendererState() {
  // Can we do this using the state stack
  glActiveTexture (GL_TEXTURE1);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_TEXTURE_GEN_S);
  glDisable (GL_TEXTURE_GEN_T);

  glActiveTexture (GL_TEXTURE0);
  glDisable (GL_TEXTURE_GEN_S);
  glDisable (GL_TEXTURE_GEN_T);

  glDisableClientState (GL_NORMAL_ARRAY);
}

void TerrainRenderer::generateAlphaTextures (Mercator::Segment * map, DataSeg &seg) {
  const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces ();
  Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin ();
  Mercator::Segment::Surfacestore::const_iterator Iend = surfaces.end ();

  for (; I != Iend; ++I) {
    if (I == surfaces.begin()) continue; // shader 0 never has alpha
    
    std::map<int, GLuint>::const_iterator J = seg.m_alphaTextures.find(I->first);
    bool validTex = (J != seg.m_alphaTextures.end());
    GLuint texNo = seg.m_alphaTextures[I->first];
    
    // if data was already good, and so is the texture, assume last state was
    // fine, and hence we've nothing to do - this should be common case 99%
    // of the time.
    if (I->second->isValid() && validTex) continue;
    
    if (!validTex) {
        // (re-)create the texture storage
        glGenTextures(1, &texNo);
        seg.m_alphaTextures[I->first] = texNo;
    }
    
    // populate if the Surface data is stale
    if (!I->second->isValid()) {
        I->second->populate();
        assert(I->second->isValid());
    }
    
    glBindTexture (GL_TEXTURE_2D, texNo);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, 65, 65, GL_ALPHA,
                     GL_UNSIGNED_BYTE, I->second->getData ());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // I wonder if this should be a mipmap?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLenum er;
    if ((er = glGetError ()) != 0) {
      std::cerr << "Booya " << gluErrorString (er)
        << std::endl << std::flush;
    }
    
    assert( glIsTexture( texNo ));
  }
}

void TerrainRenderer::drawRegion (Mercator::Segment * map,
                                  DataSeg & seg, bool select_mode) {
  // Set pointer to normal buffer
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB (GL_ARRAY_BUFFER_ARB, seg.vb_narray);
  }
  glNormalPointer (GL_FLOAT, 0, seg.narray);

  // Setup pointer to vertex buffer
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB (GL_ARRAY_BUFFER_ARB, seg.vb_harray);
  }
  glVertexPointer (3, GL_FLOAT, 0, seg.harray);

  // Clean VBO buffer
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
  }

  const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces ();
  
  Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin ();
  Mercator::Segment::Surfacestore::const_iterator Iend = surfaces.end ();
  if (select_mode) {
    // Only draw the first surface in select mode
    Iend = I;
    ++Iend;
  }

  // Use the Lock arrays extension if available.
  if (sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY]) {
    glLockArraysEXT(0, m_numLineIndeces);
  }

  for (; I != Iend; ++I) {    
    // Set up the first texture unit with the ground texture
    RenderSystem::getInstance ().switchTexture (0, m_shaders[I->first].texId);

    // Set up the second texture unit with the alpha texture
    // This is not required for the first pass, as the first pass
    // is always a fill
    if (I != surfaces.begin()) {
      glActiveTexture (GL_TEXTURE1);
      glBindTexture (GL_TEXTURE_2D, seg.m_alphaTextures[I->first]);
    }

    // Draw this segment
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                     GL_UNSIGNED_SHORT, 0);
    } else {
      glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                     GL_UNSIGNED_SHORT, m_lineIndeces);
    }

    if (I == surfaces.begin()) {
      // After the first pass, which we assume is a fill, enable
      // blending, and enable the second texture unit
      // Disable the depth write as its redundant
      glDepthMask (GL_FALSE);
      glEnable (GL_BLEND);
      glActiveTexture (GL_TEXTURE1);
      glEnable (GL_TEXTURE_2D);
    }
  }

  if (sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY]) {
    glUnlockArraysEXT();
  }


  // This restores the state we want to be in for the first pass of
  // the next segment
  glActiveTexture (GL_TEXTURE1);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
  glDepthMask (GL_TRUE);
  glActiveTexture (GL_TEXTURE0);
}

using Mercator::Terrain;

void TerrainRenderer::drawMap(Mercator::Terrain & t,
                             const PosType & camPos, bool select_mode) {

  long lowXBound = lrintf (camPos[0]) / (long)segSize - 2,
       upXBound = lrintf (camPos[0]) / (long)segSize + 2,
       lowYBound = lrintf (camPos[1]) / (long)segSize - 2,
       upYBound = lrintf (camPos[1]) / (long)segSize + 2;

  RenderSystem &rs = RenderSystem::getInstance();
//  float frustum[6][4];
//  r->getFrustum (frustum);

  const Terrain::Segmentstore & segs = t.getTerrain ();
  Terrain::Segmentstore::const_iterator I = segs.lower_bound (lowXBound);
  Terrain::Segmentstore::const_iterator K = segs.upper_bound (upXBound);

  if (I == segs.end()) return;

  if (!select_mode) enableRendererState ();

  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {

    if (glIsBufferARB(m_lineIndeces_vbo) == false) {
      glGenBuffersARB(1, &m_lineIndeces_vbo);
      glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_lineIndeces_vbo);
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_numLineIndeces * sizeof(unsigned short), m_lineIndeces, GL_STATIC_DRAW_ARB); 
    } else {
      glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, m_lineIndeces_vbo);
    }
  }


  for (; I != K; ++I) {
    const Terrain::Segmentcolumn & col = I->second;
    TerrainRenderer::DisplayListStore::iterator M = m_displayLists.find (I->first);

    Terrain::Segmentcolumn::const_iterator J = col.lower_bound (lowYBound);
    Terrain::Segmentcolumn::const_iterator L = col.upper_bound (upYBound);

  if (J == col.end()) continue;
    for (; J != L; ++J) {
      Mercator::Segment * s = J->second;

      if (s == NULL) continue;

      float min, max;
      // FIXME This test can go, once the Mercator change is in.
      if (s->isValid ()) {
        min = s->getMin ();
        max = s->getMax ();
      } else {
        // Hack. Get this data from control points
        const Mercator::Matrix<2,2, Mercator::BasePoint> &m = s->getControlPoints();
	min = std::numeric_limits<float>::max();
	max = std::numeric_limits<float>::min();
	for (int xx = 0; xx < 2; ++xx) {
          for (int yy = 0; yy < 2; ++yy) {
            min = std::min(min, m(xx,yy).height());
            max = std::max(max, m(xx,yy).height());
          }
        }
      }

      WFMath::AxisBox<3> box (WFMath::Point <3> (I->first * segSize, J->first * segSize, min), WFMath::Point < 3 > ((I->first + 1) * segSize, (J->first + 1) * segSize, max));

      if (!rs.axisBoxInFrustum (box)) {
        continue;
      }

      // Do the Frustum test.
      DisplayListColumn & dcol = (M == m_displayLists.end ())? m_displayLists[I->first] : M->second;
      DisplayListColumn::iterator N = dcol.find (J->first);

      // TerrainSegment invalidated -- lets get rid of it.
      if (!s->isValid () && N != dcol.end()) {
        N->second.contextDestroyed(true);
        dcol.erase(N);
        N  = dcol.end();
      }
      if (N == dcol.end ()) {

        if (!s->isValid ()) {
          s->populate ();
        }

        DataSeg seg;
        seg.contextCreated();
        
        // Generate normals
        seg.narray = s->getNormals(); 
        if (seg.narray == 0) {
          s->populateNormals ();
          seg.narray = s->getNormals();
        }

        // Generate normal VBO
        if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
          glGenBuffersARB (1, &seg.vb_narray);
          glBindBufferARB (GL_ARRAY_BUFFER_ARB, seg.vb_narray);
          glBufferDataARB (GL_ARRAY_BUFFER_ARB, (segSize + 1) * (segSize + 1) * 3 * sizeof (float), seg.narray, GL_STATIC_DRAW_ARB);
          seg.narray = NULL;
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        }

        // Fill in the vertex Z coord, which varies
        seg.harray = new float[(segSize + 1) * (segSize + 1) * 3];
        int idx = -1;
        for (int j = 0; j < (segSize + 1); ++j) {
          for (int i = 0; i < (segSize + 1); ++i) {
            float h = s->get (i, j);
            seg.harray[++idx] = i;
            seg.harray[++idx] = j;
            seg.harray[++idx] = h;
          }
        }

       // General vertices VBO
        if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
          glGenBuffersARB (1, &seg.vb_harray);
          glBindBufferARB (GL_ARRAY_BUFFER_ARB, seg.vb_harray);
          glBufferDataARB (GL_ARRAY_BUFFER_ARB, (segSize + 1) * (segSize +1) * 3 * sizeof (float),  seg.harray, GL_STATIC_DRAW_ARB);
          delete [] seg.harray;
          seg.harray = NULL;
          glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        }

        dcol[J->first] = seg;
        N = dcol.find(J->first);
      }
      
      DataSeg & seg = N->second;

      generateAlphaTextures (s, seg);
      
      // If we don't have VBO's fall back on display lists
      bool end = false;
      if (!sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
        if (glIsList(seg.disp)) {
          glCallList(seg.disp);
          continue;
        } else {
          seg.disp = glGenLists(1);
          dcol[J->first] = seg; // Need to re-copy updated data
          glNewList(seg.disp, GL_COMPILE);
          end = true;
        }
      }

      glPushMatrix ();
      glTranslatef (I->first * segSize, J->first * segSize, 0.0f);
      drawRegion (s, seg, select_mode);
      glPopMatrix ();

      if (end) {
        glEndList();
          glCallList(seg.disp);
      }
    }
  }
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB (GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }

  if (!select_mode) disableRendererState ();
}

void TerrainRenderer::drawSea (Mercator::Terrain & t) {
  const Terrain::Segmentstore & segs = t.getTerrain ();

  Terrain::Segmentstore::const_iterator I = segs.begin ();
  glDisable (GL_CULL_FACE);
  glEnable (GL_BLEND);
  glDisable (GL_TEXTURE_2D);
  glColor4f (0.8f, 0.8f, 1.f, 0.6f);
  glNormal3f (0.0f, 0.0f, 1.0f);
  glEnable (GL_COLOR_MATERIAL);
  float seaLevel = 0.1f * sin (System::instance ()->getTime () / 10000.0f);
  for (; I != segs.end (); ++I) {
    const Terrain::Segmentcolumn & col = I->second;
    Terrain::Segmentcolumn::const_iterator J = col.begin ();
    for (; J != col.end (); ++J) {
      Mercator::Segment * s = J->second;
if (s == NULL) continue;
      glPushMatrix ();
      glTranslatef (I->first * segSize, J->first * segSize, seaLevel);
      GLfloat vertices[] = { 0.f, 0.f, 0.f,
        segSize, 0, 0.f,
        segSize, segSize, 0.f,
        0, segSize, 0.f
      };
      glVertexPointer (3, GL_FLOAT, 0, vertices);
      glDrawArrays (GL_QUADS, 0, 4);
      glPopMatrix ();
    }
  }
  glEnable (GL_CULL_FACE);
  glDisable (GL_COLOR_MATERIAL);
  glDisable (GL_BLEND);
  glEnable (GL_TEXTURE_2D);
}

TerrainRenderer::TerrainRenderer ():
  m_terrain (Terrain::SHADED),
  m_numLineIndeces (0),
  m_lineIndeces (new unsigned short[(segSize + 1) * (segSize + 1) * 2]),
  m_lineIndeces_vbo(0),
  m_landscapeList (0),
  m_haveTerrain (false),
  m_context_no(-1)
{
  m_seaTexture    = RenderSystem::getInstance ().requestTexture ("water");
  m_shadowTexture = RenderSystem::getInstance ().requestTexture ("shadow");

  int idx = -1;
  for (unsigned int i = 0; i < (segSize + 1) - 1; ++i) {
    if (i & 1) {
      for (int j = (segSize + 1) - 1; j >= 0; --j) {
        m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
        m_lineIndeces[++idx] = j * (segSize + 1) + i;
      }
    } else {
      for (unsigned int j = 0; j < (segSize + 1); ++j) {
        m_lineIndeces[++idx] = j * (segSize + 1) + i;
        m_lineIndeces[++idx] = j * (segSize + 1) + i + 1;
      }
    }
      // if (++i >= (segSize + 1) - 1) { break; }
  }
  m_numLineIndeces = ++idx;

  // TODO set these texture names in a config file
  registerShader(new Mercator::FillShader(), "granite.png");
  registerShader(new Mercator::BandShader (-2.f, 1.5f), "sand.png");  // Sandy beach
  registerShader(new Mercator::DepthShader (0.f, -10.f), "dark.png");  // Underwater
  registerShader(new Mercator::HighShader (110.f), "snow.png");  // Snow
  registerShader(new Mercator::GrassShader (1.f, 80.f, .5f, 1.f), "rabbithill_grass_hh.png");  // Grass
}

TerrainRenderer::~TerrainRenderer() {

  RenderSystem::getInstance ().releaseTexture(m_seaTexture);
  RenderSystem::getInstance ().releaseTexture(m_shadowTexture);

  for (unsigned int i = 0; i < m_shaders.size();  ++i) {
    RenderSystem::getInstance().releaseTexture(m_shaders[i].texId);
    delete m_shaders[i].shader;
  }
  delete [] m_lineIndeces;
  contextDestroyed(true);
}

void TerrainRenderer::reset() {
  // Clear all data
  contextDestroyed(true);
  // Re-set context counter
  contextCreated();
#if(0)
  m_terrain = Mercator::Terrain(Mercator::Terrain::SHADED);
  // TODO set these texture names in a config file
  registerShader(new Mercator::FillShader(), "granite.png");
  registerShader(new Mercator::BandShader (-2.f, 1.5f), "sand.png");  // Sandy beach
  registerShader(new Mercator::DepthShader (0.f, -10.f), "dark.png");  // Underwater
  registerShader(new Mercator::HighShader (110.f), "snow.png");  // Snow
  registerShader(new Mercator::GrassShader (1.f, 80.f, .5f, 1.f), "rabbithill_grass_hh.png");  // Grass
#endif
}

void TerrainRenderer::render (const PosType & camPos, bool select_mode) {
  assert(RenderSystem::getInstance().contextValid());
  assert(m_context_no == RenderSystem::getInstance().currentContextNo());

  if (!m_haveTerrain) {
    m_haveTerrain = true;
  }
  drawMap (m_terrain, camPos, select_mode);
  if (!select_mode) {
    drawShadow (WFMath::Point < 2 > (camPos.x (), camPos.y ()), 0.5f);
  }
}

void TerrainRenderer::contextCreated() {
  assert(m_context_no == -1);

  m_context_no = RenderSystem::getInstance().currentContextNo();

  DisplayListStore::iterator I = m_displayLists.begin();
  while (I != m_displayLists.end()) {
    DisplayListColumn &dcol = (I->second);
    DisplayListColumn::iterator J = dcol.begin();
    while (J != dcol.end()) {
      (J->second).contextCreated(); 
      ++J;
    }
    ++I;
  }
}
 
void TerrainRenderer::contextDestroyed(bool check) {

  if (m_context_no == -1) return;

  DisplayListStore::iterator I = m_displayLists.begin();
  while (I != m_displayLists.end()) {
    DisplayListColumn &dcol = (I->second);
    DisplayListColumn::iterator J = dcol.begin();
    while (J != dcol.end()) {
      (J->second).contextDestroyed(check); 
      ++J;
    }
    ++I;
  }
 
  m_displayLists.clear();
 
  if (check) {
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      if (glIsBufferARB(m_lineIndeces_vbo)) {
         glDeleteBuffersARB(1, &m_lineIndeces_vbo);
      }
    }
  }
  m_lineIndeces_vbo = 0;
 
//  for (unsigned int i = 0; i < m_shaders.size();  ++i) {
    // done by texture manager?
  //  if (glIsTexture(m_shaders[i].texId)) glIsTexture(1, &m_shaders[i].texId);
//  }


  m_context_no = -1;
}

void TerrainRenderer::registerShader(Mercator::Shader* s, const std::string& texName)
{
  int index = m_shaders.size();
  m_shaders.push_back(ShaderEntry(s, texName));
  m_terrain.addShader(s, index);
  m_shaders[index].texId = RenderSystem::getInstance().requestTexture(texName);
  // assert m_shaders[index].texId is non-zero?
}

void TerrainRenderer::drawShadow (const WFMath::Point < 2 > &pos, float radius) {
  int nx = lrintf (floor (pos.x () - radius)),
      ny = lrintf (floor (pos.y () - radius)),
      fx = lrintf (ceil (pos.x () + radius)),
      fy = lrintf (ceil (pos.y () + radius));
  unsigned dx = fx - nx,
           dy = fy - ny, diameter = std::max (dx, dy), size = diameter + 1;
           fx = nx + diameter;
           fy = ny + diameter;
  float *vertices = new float[size * size * 3];
  float *texcoords = new float[size * size * 2];
  float *vptr = vertices - 1;
  float *tptr = texcoords - 1;
  for (int y = ny; y <= fy; ++y) {
    for (int x = nx; x <= fx; ++x)  {
      *++vptr = x;
      *++vptr = y;
      *++vptr = m_terrain.get (x, y);
      *++tptr = ((float) x - pos.x () + radius) / (radius * 2);
      *++tptr = ((float) y - pos.y () + radius) / (radius * 2);
    }
  }
  GLushort *indices = new GLushort[diameter * size * 2];
  GLushort *iptr = indices - 1;
  int numind = 0;
  for (GLuint i = 0; i < diameter; ++i)  {
    // This ensures that we are drawing the same triangles
    // in the same order as they are done in the original terrain
    // passes
    if ((i + nx) & 1) {
      for (GLshort j = diameter; j >= 0; --j) {
        *++iptr = j * size + i + 1;
        *++iptr = j * size + i;
        numind += 2;
      }
    } else {
      for (GLuint j = 0; j <= diameter; ++j) {
        *++iptr = j * size + i;
        *++iptr = j * size + i + 1;
        numind += 2;
      }
    }
  }

  RenderSystem::getInstance ().switchTexture (0, m_shadowTexture);

  glEnable (GL_TEXTURE_2D);
  glEnable (GL_BLEND);

  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  glVertexPointer (3, GL_FLOAT, 0, vertices);
  glTexCoordPointer (2, GL_FLOAT, 0, texcoords);
  glDepthMask (GL_FALSE);
  glDrawElements (GL_TRIANGLE_STRIP, numind, GL_UNSIGNED_SHORT, indices);
  glDepthMask (GL_TRUE);

  glDisableClientState (GL_TEXTURE_COORD_ARRAY);
  glDisable (GL_BLEND);
//    glDisable(GL_TEXTURE_2D);

  delete [] vertices;
  delete [] texcoords;
  delete [] indices;
}

} /* namespace Sear */
