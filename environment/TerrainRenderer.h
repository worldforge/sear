// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch, Simon Goodall

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
        if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
          if (glIsBufferARB(vb_narray)) {
            glDeleteBuffersARB(1, &vb_narray);
          }
          vb_narray = 0;
  
          if (glIsBufferARB(vb_harray)) {
            glDeleteBuffersARB(1, &vb_harray);
          }
          vb_harray = 0;
        }
        if (glIsList(disp)) {
          glDeleteLists(1, disp);
        }
        disp = 0;

        // Clean up buffers
        if (harray) delete [] harray;
        harray = NULL;
        narray = NULL;

        // Clean up textures
        if (glIsTexture(m_alphaTextures[0])) {
          glDeleteTextures(8, m_alphaTextures);
        }
        for (int i = 0; i < 8; m_alphaTextures[i++] = 0);
      }
    };
    typedef std::map<int, DataSeg> DisplayListColumn;
    typedef std::map<int, DisplayListColumn> DisplayListStore;

    Mercator::Terrain m_terrain;

    class ShaderEntry
    {
    public:
        ShaderEntry(Mercator::Shader* s, const std::string& tnm) :
            shader(s),
            texId(0),
            texName(tnm)
        {}
        
        Mercator::Shader* shader;
        GLint texId;
        std::string texName; // for invalidation
    };

    std::vector<ShaderEntry> m_shaders;
    
    void invalidate();
  protected:
    DisplayListStore m_displayLists;
    int m_numLineIndeces;
    unsigned short * const m_lineIndeces;
   
    int m_seaTexture;
    int m_shadowTexture;
    GLuint m_landscapeList;
    bool m_haveTerrain;

    void enableRendererState();
    void disableRendererState();

    void generateAlphaTextures(Mercator::Segment *, DataSeg &);
    void drawRegion(Mercator::Segment *, DataSeg&, bool select_mode);
    void drawMap(Mercator::Terrain &, const PosType & camPos, bool select_mode);
    void drawSea( Mercator::Terrain &);
    void drawShadow(const WFMath::Point<2> & pos, float radius = 1.f);
    void readTerrain();
  public:
    TerrainRenderer();
    virtual ~TerrainRenderer();
    
    virtual void render( const PosType & camPos, bool select_mode);
    virtual void renderSea() { drawSea(m_terrain); }
    friend class Environment;
    
    void registerShader(Mercator::Shader*, const std::string& texId);
};
}
#endif // APOGEE_TERRAIN_RENDERER_H
