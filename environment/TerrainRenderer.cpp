// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "TerrainRenderer.h"

//#include "Texture.h"
#include <sage/GLU.h>


#include <Mercator/Segment.h>
#include <Mercator/FillShader.h>
#include <Mercator/ThresholdShader.h>
#include <Mercator/DepthShader.h>
#include <Mercator/GrassShader.h>
#include <Mercator/Surface.h>

#include <iostream>


#include "src/System.h"
#include "src/Graphics.h"
#include "src/Render.h"


namespace Sear {

static const bool debug_flag = false;
static const int segSize = 64;

static GLfloat sx0[] = {0.125f, 0.f, 0.f, 0.f};
static GLfloat ty0[] = {0.f, 0.125f, 0.f, 0.f};

static GLfloat sx1[] = {0.015625f, 0.f, 0.f, 0.f};
static GLfloat ty1[] = {0.f, 0.015625f, 0.f, 0.f};

void TerrainRenderer::enableRendererState()
{
    static const float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    static const float diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    static const float specular[] = { 0.f, 0.f, 0.f, 1.0f };
    static const float emission[] = { 0.f, 0.f, 0.f, 1.0f };
    static const float shininess[] = { 0.0f };
    glEnable(GL_NORMALIZE);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glEnableClientState(GL_NORMAL_ARRAY);
//     glAlphaFunc(GL_GREATER, 0.5f);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_ALPHA_TEST);

        glActiveTexture(GL_TEXTURE1);
    // glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx1);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, ty1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sx0);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, ty0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


}

void TerrainRenderer::disableRendererState()
{
    // Can we do this using the state stack

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_NORMALIZE);
}

void TerrainRenderer::generateAlphaTextures(Mercator::Segment * map)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces();
    Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin();

    glGenTextures(surfaces.size(), m_alphaTextures);
    // FIXME These textures we have allocated are leaked.
    for (int texNo = 0; I != surfaces.end(); ++I, ++texNo) {
        if ((!(*I)->m_shader.checkIntersect(**I)) || (texNo == 0)) {
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, m_alphaTextures[texNo]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, 65, 65, GL_ALPHA,
                          GL_UNSIGNED_BYTE, (*I)->getData());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // I wonder if this should be a mipmap?
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        GLenum er;
        if ((er = glGetError()) != 0) {
            std::cerr << "Booya " << gluErrorString(er)
                      << std::endl << std::flush;
        }
    }
}

void TerrainRenderer::drawRegion(Mercator::Segment * map)
{
    float harray[(segSize+1)*(segSize+1)*3];
    float * narray = map->getNormals();
    if (narray == 0) {
        std::cout << "Populating normals" << std::endl << std::flush;
        map->populateNormals();
        narray = map->getNormals();
    }
    // Fill in the vertex Z coord, which varies
    std::cout << "Populating vertex cache" << std::endl << std::flush;
    int idx = -1;
    for(int j = 0; j < (segSize + 1); ++j) {
        for(int i = 0; i < (segSize + 1); ++i) {
            float h = map->get(i,j);
            harray[++idx] = i;
            harray[++idx] = j;
            harray[++idx] = h;
        }
    }
    glNormalPointer(GL_FLOAT, 0, narray);
    glVertexPointer(3, GL_FLOAT, 0, harray);

    const Mercator::Segment::Surfacestore & surfaces = map->getSurfaces();
    Mercator::Segment::Surfacestore::const_iterator I = surfaces.begin();

    for (int texNo = 0; I != surfaces.end(); ++I, ++texNo) {
        // Do a rough check to see if this pass applies to this segment
        if (!(*I)->m_shader.checkIntersect(**I)) {
            continue;
        }

        // Set up the first texture unit with the ground texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textures[texNo]);

        // Set up the second texture unit with the alpha texture
        // This is not required for the first pass, as the first pass
        // is always a fill
        if (texNo != 0) {
           glActiveTexture(GL_TEXTURE1);
           glBindTexture(GL_TEXTURE_2D, m_alphaTextures[texNo]);
        }
                     
        // Draw this segment
        glDrawElements(GL_TRIANGLE_STRIP, m_numLineIndeces,
                       GL_UNSIGNED_SHORT, m_lineIndeces);

        if (texNo == 0) {
            // After the first pass, which we assume is a fill, enable
            // blending, and enable the second texture unit
            // Disable the depth write as its redundant
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glActiveTexture(GL_TEXTURE1);
            glEnable(GL_TEXTURE_2D);
        }
    }

    // This restores the state we want to be in for the first pass of
    // the next segment
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

}

using Mercator::Terrain;

void TerrainRenderer::drawMap(Mercator::Terrain & t,
                              const PosType & camPos)
{
    long lowXBound = lrintf(camPos[0] / segSize) - 2,
         upXBound = lrintf(camPos[0] / segSize) + 2,
         lowYBound = lrintf(camPos[1] / segSize) - 2,
         upYBound = lrintf(camPos[1] / segSize) + 2;

    enableRendererState();

    const Terrain::Segmentstore & segs = t.getTerrain();

    Terrain::Segmentstore::const_iterator I = segs.lower_bound(lowXBound);
    Terrain::Segmentstore::const_iterator K = segs.upper_bound(upXBound);
    for (; I != K; ++I) {
        const Terrain::Segmentcolumn & col = I->second;
        TerrainRenderer::DisplayListStore::iterator M = m_displayLists.find(I->first);

        Terrain::Segmentcolumn::const_iterator J = col.lower_bound(lowYBound);
        Terrain::Segmentcolumn::const_iterator L = col.upper_bound(upYBound);
        for (; J != L; ++J) {
            DisplayListColumn & dcol = (M == m_displayLists.end()) ? 
                                           m_displayLists[I->first] :
                                           M->second;
            DisplayListColumn::const_iterator N = dcol.find(J->first);
            GLuint display_list;
            if (N != dcol.end()) {
  //              debug(std::cout << "Using display list for "
//                                << I->first << ", " << J->first
//                                << std::endl << std::flush;);
                display_list = N->second;
            } else {
            //    debug(std::cout << "Building display list for "
//                                << I->first << ", " << J->first
//                                << std::endl << std::flush;);


                Mercator::Segment * s = J->second;
                if (!s->isValid()) {
                    s->populate();
                }
                Mercator::Segment::Surfacestore & surfaces = s->getSurfaces();
                if (!surfaces.empty() && !surfaces.front()->isValid()) {
                    s->populateSurfaces();
                }

                generateAlphaTextures(s);

                display_list = glGenLists(1);
                glNewList(display_list, GL_COMPILE);

                glPushMatrix();
                glTranslatef(I->first * segSize, J->first * segSize, 0.0f);

                drawRegion(s);
                s->invalidate(false);
                glPopMatrix();
    
                glEndList();
                dcol[J->first] = display_list;
            }

            if (!glIsList(display_list)) {
                std::cout << "Display list for terrain segment " << I->first
                          << "," << J->first
                          << " is no longer a display list in this context" 
                          << std::endl << std::flush;
            }
            glCallList(display_list);
        }
    }
    disableRendererState();

}

void TerrainRenderer::drawSea( Mercator::Terrain & t)
{
    const Terrain::Segmentstore & segs = t.getTerrain();

    Terrain::Segmentstore::const_iterator I = segs.begin();
    glEnable(GL_BLEND);
    for (; I != segs.end(); ++I) {
        const Terrain::Segmentcolumn & col = I->second;
        Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, 0.0f);
            GLfloat vertices[] = { 0.f, 0.f, 0.f,
                                   segSize, 0, 0.f,
                                   segSize, segSize, 0.f,
                                   0, segSize, 0.f };
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glColor4f(0.8f, 0.8f, 1.f, 0.6f);
            glDrawArrays(GL_QUADS, 0, 4);
            glPopMatrix();
        }
    }
    glDisable(GL_BLEND);
}

TerrainRenderer::TerrainRenderer() :
m_terrain(Terrain::SHADED),
    m_numLineIndeces(0),
    m_lineIndeces(new unsigned short[(segSize + 1) * (segSize + 1) * 2]),
    m_landscapeList(0), m_haveTerrain(false)

{

m_textures[0] = System::instance()->getGraphics()->getRender()->requestTexture("granite.png");
m_textures[0] = System::instance()->getGraphics()->getRender()->requestTexture("sand.png");
m_textures[0] = System::instance()->getGraphics()->getRender()->requestTexture("rabbithill_grass_hh.png");
m_textures[0] = System::instance()->getGraphics()->getRender()->requestTexture("dark.png");
m_textures[0] = System::instance()->getGraphics()->getRender()->requestTexture("snow.png");
//    m_textures[0] = Texture::get("granite.png", true, GL_LINEAR_MIPMAP_NEAREST);
//    m_textures[1] = Texture::get("sand.png", true, GL_LINEAR_MIPMAP_NEAREST);
//    m_textures[2] = Texture::get("rabbithill_grass_hh.png", true, GL_LINEAR_MIPMAP_NEAREST);
//    m_textures[3] = Texture::get("dark.png", true, GL_LINEAR_MIPMAP_NEAREST);
//    m_textures[4] = Texture::get("snow.png", true, GL_LINEAR_MIPMAP_NEAREST);

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

    m_terrain.addShader(new Mercator::FillShader());
    m_terrain.addShader(new Mercator::BandShader(-2.f, 1.5f)); // Sandy beach
    m_terrain.addShader(new Mercator::GrassShader(1.f, 27.f, .5f, 1.f)); // Grass
    m_terrain.addShader(new Mercator::DepthShader(0.f, -10.f)); // Underwater
    m_terrain.addShader(new Mercator::HighShader(35.f)); // Snow
}

TerrainRenderer::~TerrainRenderer()
{
}

void TerrainRenderer::render( const PosType & camPos)
{
    if (!m_haveTerrain) {
        m_haveTerrain = true;
    }
    drawMap(m_terrain, camPos);
    drawSea( m_terrain);
//    drawShadow(WFMath::Point<2>(camPos.x(), camPos.y()), .5f);
}

void TerrainRenderer::select( const PosType & camPos)
{
    drawMap(m_terrain, camPos);
    // selectTerrain(m_terrain);
}

}
