// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "TerrainRenderer.h"

#include "renderers/RenderSystem.h"

#include "src/Render.h"

#include <sage/GLU.h>

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
    static const float shininess[] = { 0.1f };
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
//    glEnable(GL_TEXTURE_2D);
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
//    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glDisableClientState(GL_NORMAL_ARRAY);
    //glDisable(GL_NORMALIZE);
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
    float * harray = new float[(segSize+1)*(segSize+1)*3];
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
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, m_textures[texNo]);
        RenderSystem::getInstance().switchTexture(0, m_textures[texNo]);

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

    delete [] harray;

    // This restores the state we want to be in for the first pass of
    // the next segment
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glActiveTexture(GL_TEXTURE0);

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

    Render * r = RenderSystem::getInstance().getRenderer();
    float frustum[6][4];
    r->getFrustum(frustum);

    const Terrain::Segmentstore & segs = t.getTerrain();

    Terrain::Segmentstore::const_iterator I = segs.lower_bound(lowXBound);
    Terrain::Segmentstore::const_iterator K = segs.upper_bound(upXBound);
    for (; I != K; ++I) {
        const Terrain::Segmentcolumn & col = I->second;
        TerrainRenderer::DisplayListStore::iterator M = m_displayLists.find(I->first);

        Terrain::Segmentcolumn::const_iterator J = col.lower_bound(lowYBound);
        Terrain::Segmentcolumn::const_iterator L = col.upper_bound(upYBound);
        for (; J != L; ++J) {
            Mercator::Segment * s = J->second;
            float min, max;
            // FIXME This test can go, once the Mercator change is in.
            if (s->isValid()) {
                min = s->getMin();
                max = s->getMax();
            } else {
                // Hack. Get this data from control points
                min = 0;
                max = 1;
            }

            WFMath::AxisBox<3> box(WFMath::Point<3>(I->first * segSize,
                                                    J->first * segSize,
                                                    min),
                                   WFMath::Point<3>((I->first + 1) * segSize,
                                                    (J->first + 1) * segSize,
                                                    max));

            if (!r->patchInFrustum(box)) {
                continue;
            }
            
            // Do the Frustum test.
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
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.8f, 0.8f, 1.f, 0.6f);
    glNormal3f(0.0f,0.0f,1.0f);
    glEnable(GL_COLOR_MATERIAL);
    float seaLevel = 0.1f * sin(System::instance()->getTime() / 10000.0f);
    for (; I != segs.end(); ++I) {
        const Terrain::Segmentcolumn & col = I->second;
        Terrain::Segmentcolumn::const_iterator J = col.begin();
        for (; J != col.end(); ++J) {
            glPushMatrix();
            glTranslatef(I->first * segSize, J->first * segSize, seaLevel);
            GLfloat vertices[] = { 0.f, 0.f, 0.f,
                                   segSize, 0, 0.f,
                                   segSize, segSize, 0.f,
                                   0, segSize, 0.f };
            glVertexPointer(3, GL_FLOAT, 0, vertices);
            glDrawArrays(GL_QUADS, 0, 4);
            glPopMatrix();
        }
    }
    glEnable(GL_CULL_FACE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
}

TerrainRenderer::TerrainRenderer() :
    m_terrain(Terrain::SHADED),
    m_numLineIndeces(0),
    m_lineIndeces(new unsigned short[(segSize + 1) * (segSize + 1) * 2]),
    m_landscapeList(0), m_haveTerrain(false)
{

    m_textures[0] = RenderSystem::getInstance().requestTexture("granite.png");
    m_textures[1] = RenderSystem::getInstance().requestTexture("sand.png");
    m_textures[2] = RenderSystem::getInstance().requestTexture("rabbithill_grass_hh.png");
    m_textures[3] = RenderSystem::getInstance().requestTexture("dark.png");
    m_textures[4] = RenderSystem::getInstance().requestTexture("snow.png");
    m_seaTexture = RenderSystem::getInstance().requestTexture("water");
    m_shadowTexture = RenderSystem::getInstance().requestTexture("shadow");

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
    // TODO these are leaked
    m_terrain.addShader(new Mercator::FillShader());
    m_terrain.addShader(new Mercator::BandShader(-2.f, 1.5f)); // Sandy beach
    m_terrain.addShader(new Mercator::GrassShader(1.f, 80.f, .5f, 1.f)); // Grass
    m_terrain.addShader(new Mercator::DepthShader(0.f, -10.f)); // Underwater
    m_terrain.addShader(new Mercator::HighShader(110.f)); // Snow
}

void TerrainRenderer::render( const PosType & camPos)
{
    if (!m_haveTerrain) {
        m_haveTerrain = true;
    }
    drawMap(m_terrain, camPos);
    drawShadow(WFMath::Point<2>(camPos.x(), camPos.y()), .5f);
}

void TerrainRenderer::drawShadow(const WFMath::Point<2> & pos, float radius)
{
    int nx = lrintf(floor(pos.x() - radius)),
        ny = lrintf(floor(pos.y() - radius)),
        fx = lrintf(ceil(pos.x() + radius)),
        fy = lrintf(ceil(pos.y() + radius));
    unsigned dx = fx - nx,
             dy = fy - ny,
             diameter = std::max(dx, dy),
             size = diameter + 1;
    fx = nx + diameter;
    fy = ny + diameter;
    float * vertices = new float[size * size * 3];
    float * texcoords = new float[size * size * 2];
    float * vptr = vertices - 1;
    float * tptr = texcoords - 1;
    for(int y = ny; y <= fy; ++y) {
        for(int x = nx; x <= fx; ++x) {
            *++vptr = x;
            *++vptr = y;
            *++vptr = m_terrain.get(x, y);
            *++tptr = ((float)x - pos.x() + radius) / (radius * 2);
            *++tptr = ((float)y - pos.y() + radius) / (radius * 2);
        }
    }
    GLushort * indices = new GLushort[diameter * size * 2];
    GLushort * iptr = indices - 1;
    int numind = 0;
    for(GLuint i = 0; i < diameter; ++i) {
        // This ensures that we are drawing the same triangles
        // in the same order as they are done in the original terrain
        // passes
        if ((i + nx) & 1) {
            for(GLshort j = diameter; j >= 0; --j) {
                *++iptr = j * size + i + 1;
                *++iptr = j * size + i;
                numind += 2;
            }
        } else {
            for(GLuint j = 0; j <= diameter; ++j) {
                *++iptr = j * size + i;
                *++iptr = j * size + i + 1;
                numind += 2;
            }
        }
    }
//    GLuint shTexture = Texture::get("shadow.png", false);
    RenderSystem::getInstance().switchTexture(0, m_shadowTexture);
   
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
//    glBindTexture(GL_TEXTURE_2D, shTexture);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                                                                                
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
            glDepthMask(GL_FALSE);
    glDrawElements(GL_TRIANGLE_STRIP, numind, GL_UNSIGNED_SHORT, indices);
            glDepthMask(GL_TRUE);
                                                                                
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_BLEND);
//    glDisable(GL_TEXTURE_2D);
                                                                                
                                                                                
}

} /* namespace Sear */
