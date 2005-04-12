// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch, Simon Goodall

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TerrainRenderer.h"

#include "renderers/RenderSystem.h"

#include "renderers/Render.h"

#include <sage/GLU.h>
#include <sage/GL.h>

#include "src/System.h"
#include <Mercator/Segment.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <iostream>

#ifdef USE_MMGR
#include "common/mmgr.h"
#endif

namespace Sear {

static const bool debug_flag = false;
static const int segSize = 64;

static GLfloat sx0[] = { 0.125f, 0.f, 0.f, 0.f };
static GLfloat ty0[] = { 0.f, 0.125f, 0.f, 0.f };

static GLfloat sx1[] = { 0.015625f, 0.f, 0.f, 0.f };
static GLfloat ty1[] = { 0.f, 0.015625f, 0.f, 0.f };

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
  // Required otherwise alpha textures behave strangely
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces ();
  Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin ();

  glGenTextures (surfaces.size (), seg.m_alphaTextures);
  // FIXME These textures we have allocated are leaked.
  for (int texNo = 0; I != surfaces.end(); ++I, ++texNo) {
    if (texNo == 0) continue; // shader 0 never has alpah
    
    glBindTexture (GL_TEXTURE_2D, seg.m_alphaTextures[texNo]);
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
  }
}

void TerrainRenderer::drawRegion (Mercator::Segment * map, DataSeg & seg) {
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

  for (int texNo = 0; I != surfaces.end (); ++I, ++texNo) {
           
    // Set up the first texture unit with the ground texture
    RenderSystem::getInstance ().switchTexture (0, m_textures[I->first]);

    // Set up the second texture unit with the alpha texture
    // This is not required for the first pass, as the first pass
    // is always a fill
    if (texNo != 0) {
      glActiveTexture (GL_TEXTURE1);
      glBindTexture (GL_TEXTURE_2D, seg.m_alphaTextures[texNo]);
    }

    // Draw this segment
    glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                   GL_UNSIGNED_SHORT, m_lineIndeces);

    if (texNo == 0) {
      // After the first pass, which we assume is a fill, enable
      // blending, and enable the second texture unit
      // Disable the depth write as its redundant
      glDepthMask (GL_FALSE);
      glEnable (GL_BLEND);
      glActiveTexture (GL_TEXTURE1);
      glEnable (GL_TEXTURE_2D);
    }
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
                             const PosType & camPos) {
  long lowXBound = lrintf (camPos[0] / segSize) - 2,
       upXBound = lrintf (camPos[0] / segSize) + 2,
       lowYBound = lrintf (camPos[1] / segSize) - 2,
       upYBound = lrintf (camPos[1] / segSize) + 2;

  enableRendererState ();

  Render *r = RenderSystem::getInstance ().getRenderer ();
  float frustum[6][4];
  r->getFrustum (frustum);

  const Terrain::Segmentstore & segs = t.getTerrain ();
  Terrain::Segmentstore::const_iterator I = segs.lower_bound (lowXBound);
  Terrain::Segmentstore::const_iterator K = segs.upper_bound (upXBound);

  for (; I != K; ++I) {
    const Terrain::Segmentcolumn & col = I->second;
    TerrainRenderer::DisplayListStore::iterator M =
    m_displayLists.find (I->first);

    Terrain::Segmentcolumn::const_iterator J = col.lower_bound (lowYBound);
    Terrain::Segmentcolumn::const_iterator L = col.upper_bound (upYBound);
    for (; J != L; ++J) {
      Mercator::Segment * s = J->second;
      float min, max;
      // FIXME This test can go, once the Mercator change is in.
      if (s->isValid ()) {
        min = s->getMin ();
        max = s->getMax ();
      } else {
        // Hack. Get this data from control points
        min = 0;
        max = 1;
      }

      WFMath::AxisBox<3> box (WFMath::Point <3> (I->first * segSize, J->first * segSize, min), WFMath::Point < 3 > ((I->first + 1) * segSize, (J->first + 1) * segSize, max));

      if (!r->patchInFrustum (box)) {
        continue;
      }

      // Do the Frustum test.
      DisplayListColumn & dcol = (M == m_displayLists.end ())? m_displayLists[I->first] : M->second;
      DisplayListColumn::iterator N = dcol.find (J->first);
//            GLuint display_list;
      DataSeg seg;
      if (N != dcol.end ()) {
        seg = N->second;
      } else {
        if (!s->isValid ()) {
          s->populate ();
        }
        Mercator::Segment::Surfacestore & surfaces = s->getSurfaces ();
        if (!surfaces.empty () && !surfaces.begin()->second->isValid ()) {
          s->populateSurfaces ();
        }
        // Generate the alpha textures for each shader for this surface
        generateAlphaTextures (s, seg);

        // Generate normsl
        seg.narray = s->getNormals (); 
        if (seg.narray == 0) {
//          std::cout << "Populating normals" << std::endl << std::flush;
          s->populateNormals ();
          seg.narray = s->getNormals ();
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
//        std::cout << "Populating vertex cache" << std::endl << std::flush;
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
//        s->invalidate (false);
      }
      // If we don't have VBO's fall back on display lists
      bool end = false;
      if (!sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
        if (glIsList(seg.disp)) {
          glCallList(seg.disp);
          continue;
        } else {
          seg.disp = glGenLists(1);
          dcol[J->first] = seg; // Need to re-copy updated data
          glNewList(seg.disp, GL_COMPILE_AND_EXECUTE);
          end = true;
        }
      }

      glPushMatrix ();
      glTranslatef (I->first * segSize, J->first * segSize, 0.0f);
      drawRegion (s, seg);
      glPopMatrix ();

      if (end) {
        glEndList();
      }
    }
  }

  disableRendererState ();
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
  m_landscapeList (0),
  m_haveTerrain (false)
{
  // Get texture id's
  m_textures[0]   = RenderSystem::getInstance ().requestTexture ("granite.png");
  m_textures[1]   = RenderSystem::getInstance ().requestTexture ("sand.png");
  m_textures[2]   = RenderSystem::getInstance ().requestTexture ("rabbithill_grass_hh.png");
  m_textures[3]   = RenderSystem::getInstance ().requestTexture ("dark.png");
  m_textures[4]   = RenderSystem::getInstance ().requestTexture ("snow.png");
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

  // Creat our shaders here so we can clean then up later
  m_shaders.push_back (new Mercator::FillShader ());
  m_shaders.push_back (new Mercator::BandShader (-2.f, 1.5f));  // Sandy beach
  m_shaders.push_back (new Mercator::GrassShader (1.f, 80.f, .5f, 1.f));  // Grass
  m_shaders.push_back (new Mercator::DepthShader (0.f, -10.f));  // Underwater
  m_shaders.push_back (new Mercator::HighShader (110.f));  // Snow

  // Add to mercator terrain
  for (unsigned int i = 0; i < m_shaders.size (); ++i) {
    m_terrain.addShader (m_shaders[i], i);
  }
}

void TerrainRenderer::render (const PosType & camPos) {
  if (!m_haveTerrain) {
    m_haveTerrain = true;
  }
  drawMap (m_terrain, camPos);
  drawShadow (WFMath::Point < 2 > (camPos.x (), camPos.y ()), .5f);
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
//    GLuint shTexture = Texture::get("shadow.png", false);
  RenderSystem::getInstance ().switchTexture (0, m_shadowTexture);

  glEnable (GL_TEXTURE_2D);
  glEnable (GL_BLEND);
//    glBindTexture(GL_TEXTURE_2D, shTexture);

  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  glVertexPointer (3, GL_FLOAT, 0, vertices);
  glTexCoordPointer (2, GL_FLOAT, 0, texcoords);
  glDepthMask (GL_FALSE);
  glDrawElements (GL_TRIANGLE_STRIP, numind, GL_UNSIGNED_SHORT, indices);
  glDepthMask (GL_TRUE);

  glDisableClientState (GL_TEXTURE_COORD_ARRAY);
  glDisable (GL_BLEND);
//    glDisable(GL_TEXTURE_2D);
}

} /* namespace Sear */
