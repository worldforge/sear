// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef APOGEE_TERRAIN_RENDERER_H
#define APOGEE_TERRAIN_RENDERER_H

#include <vector>

#include <sage/sage.h>
#include <sage/GL.h>

#include <Mercator/Terrain.h>
#include <Mercator/Shader.h>
#include <wfmath/point.h>

namespace Sear {

class Environment;

typedef WFMath::Point<3> PosType;


class TerrainRenderer 
{
  public:
    class DataSeg {
public:
      DataSeg() :
        vb_narray(0),
        vb_harray(0),
        disp(0),
        harray(NULL),
        narray(NULL)
      {}
      ~DataSeg() {}
      GLuint m_alphaTextures[8];
      GLuint vb_narray;
      GLuint vb_harray;
      GLuint disp;
      float *harray;
      float *narray;

      void invalidate() {
        if (glIsBufferARB(vb_narray)) {
          glDeleteBuffersARB(1, &vb_narray);
        }
        vb_narray = 0;

        if (glIsBufferARB(vb_harray)) {
          glDeleteBuffersARB(1, &vb_harray);
        }
        vb_harray = 0;

        if (glIsList(disp)) {
          glDeleteLists(1, disp);
        }
        disp = 0;

        if (harray) delete [] harray;
      }
    };
    typedef std::map<int, DataSeg> DisplayListColumn;
    typedef std::map<int, DisplayListColumn> DisplayListStore;

    Mercator::Terrain m_terrain;

    void invalidate() {
      DisplayListStore::iterator I = m_displayLists.begin();
      while (I != m_displayLists.end()) {
        DisplayListColumn &dcol = (I->second);
        DisplayListColumn::iterator J = dcol.begin();
        while (J != dcol.end()) {
          (J->second).invalidate(); 
          ++J;
        }
        ++I;
      }
     
      m_displayLists.clear();
    }

  protected:
    DisplayListStore m_displayLists;
    int m_numLineIndeces;
    unsigned short * const m_lineIndeces;
    int m_textures[8];
    int m_seaTexture;
    int m_shadowTexture;
    GLuint m_landscapeList;
    bool m_haveTerrain;
    std::vector<Mercator::Shader*> m_shaders;

    void enableRendererState();
    void disableRendererState();

    void generateAlphaTextures(Mercator::Segment *, DataSeg &);
    void drawRegion(Mercator::Segment *, DataSeg&);
    void drawMap(Mercator::Terrain &, const PosType & camPos);
    void drawSea( Mercator::Terrain &);
    void drawShadow(const WFMath::Point<2> & pos, float radius = 1.f);
    void readTerrain();
  public:
    TerrainRenderer();
    virtual ~TerrainRenderer() {
      for (unsigned int i = 0; i < m_shaders.size();  ++i) {
        delete m_shaders[i];
      }
      delete [] m_lineIndeces;
      invalidate();
    }

    virtual void render( const PosType & camPos);
    virtual void renderSea() { drawSea(m_terrain); }
    friend class Environment;
};
}
#endif // APOGEE_TERRAIN_RENDERER_H
