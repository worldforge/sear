#include <sage/sage.h>
#include <sage/GL.h>
#include <math.h>
#include <iostream>
#include "src/Render.h"
#include "src/System.h"
#include "src/Graphics.h"

#include "renderers/RenderSystem.h"

#define SQR(X) ((X))

#define zFac(z) 1.0f 
//#define zFac(z)  (  sin(  z) * sin(z))

#include "SkyDome.h"

namespace Sear {

float *SkyDome::genVerts(float radius, int levels, int segments) {
  int size = segments * levels;
  int vert_counter = -1;
  float *verts = new float[size * 3 * 4];
  float levelInc = M_PI / (float)levels / 2.0f;
  float segmentInc = (2.0f * M_PI) / (float)segments;
  for (int i = 0; i < levels; ++i) {
    float a2 = ((float)i * levelInc);
    float a22 = ((float)(i + 1) * levelInc);

    for (int j = 0; j < segments; ++j) {
      float a1 = ((float)j * segmentInc);
      float a11 = ((float)(j + 1) * segmentInc);

      float x,y,z;
      
      x = sin(a1) * sin(a2) * SQR(radius);
      y = cos(a1) * sin(a2) * SQR(radius);
      z = cos(a2) * radius;

      verts[++vert_counter] = x;
      verts[++vert_counter] = y;
      verts[++vert_counter] = z;
      
      x = sin(a11) * sin(a2) * SQR(radius);
      y = cos(a11) * sin(a2) * SQR(radius);
      z = cos(a2) * radius;

      verts[++vert_counter] = x;
      verts[++vert_counter] = y;
      verts[++vert_counter] = z;
      
      x = sin(a11) * sin(a22) * SQR(radius);
      y = cos(a11) * sin(a22) * SQR(radius);
      z = cos(a22) * radius;

      verts[++vert_counter] = x;
      verts[++vert_counter] = y;
      verts[++vert_counter] = z;
      
      x = sin(a1) * sin(a22) * SQR(radius);
      y = cos(a1) * sin(a22) * SQR(radius);
      z = cos(a22) * radius;

      verts[++vert_counter] = x;
      verts[++vert_counter] = y;
      verts[++vert_counter] = z;
    }
  }
  return verts;
}
 
float *SkyDome::genTexCoordsA(float radius, int levels, int segments)  {
  int size = segments * levels;
  float *tex = new float[size * 2 * 4];
  int tex_counter = -1;

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

float *SkyDome::genTexCoordsB(float radius, int levels, int segments)  {
  int size = segments * levels;
  int tex_counter = -1;
  float *tex = new float[size * 2 * 4];
  float levelInc = M_PI / (float)levels / 2.0f;
  float segmentInc = (2.0f * M_PI) / (float)segments;
  for (int i = 0; i < levels; ++i) {
    float a2 = ((float)i * levelInc);
    float a22 = ((float)(i + 1) * levelInc);

    for (int j = 0; j < segments; ++j) {
      float a1 = ((float)j * segmentInc);
      float a11 = ((float)(j + 1) * segmentInc);

      float x,y,z;
      
      z = zFac(a2);

      x = sin(a1) * z;
      y = -cos(a1) * z;
z = exp(1.0-x) * exp(1.0-y);
      tex[++tex_counter] = x / z;
      tex[++tex_counter] = y/z;
      
      z = 1.0f;;//zFac(a2);

      x = sin(a11) * z;
      y = -cos(a11) * z;
z = exp(1.0-x) * exp(1.0-y);

      tex[++tex_counter] = (x )/z;
      tex[++tex_counter] = (y )/z;
      
      z = 1.0f;;//zFac(a22);

      x = sin(a11) * z;
      y = -cos(a11) * z;
z = exp(1.0-x) * exp(1.0-y);
      tex[++tex_counter] = (x )/z;
      tex[++tex_counter] = (y )/z;
      
      z =1.0f;;// zFac(a22);

      x = sin(a1) * z;
      y = -cos(a1) * z;

z = exp(1.0-x) * exp(1.0-y);

      tex[++tex_counter] = (x )/z;
      tex[++tex_counter] = (y )/z;
    }
  }
  return tex;
} 

void SkyDome::domeInit(float radius, int levels, int segments) {
//  if (!LoadTGA(&m_textures[0],"clearSky2.tga", false)
 //   || !LoadTGA(&m_textures[1],"glow1.tga",false)
  //  || !LoadTGA(&m_textures[2],"clouds2.tga",true)
   // || !LoadTGA(&m_textures[3],"clouds3.tga",true)
    //|| !LoadTGA(&m_textures[4],"stars.tga",true)
  //  ) // Load The Font Texture
 // {
  //  std::cerr << "Error loading sky textures" << std::endl;
  //}
  m_textures[0] = RenderSystem::getInstance().requestTexture("atmosphere");
//  m_RenderSystem::getInstance().requestTexture("glow");
  m_textures[1] = RenderSystem::getInstance().requestTexture("cloud_layer_1");
  m_textures[2] = RenderSystem::getInstance().requestTexture("cloud_layer_2");
//  m_tRenderSystem::getInstance().requestTexture("cloud_layer_2");
//  m_RenderSystem::getInstance().requestTexture("star_field");

  m_verts = genVerts(radius, levels, segments);  
  m_texA = genTexCoordsA(radius, levels, segments);  
  m_texB = genTexCoordsB(radius, levels, segments);  
//  texC = genTexCoordsB(radius, levels, segments);

  int size = segments * levels;

  glGenBuffersARB(1, &m_vb_verts);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_verts);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * 3 * 4 * sizeof(float), m_verts, GL_STATIC_DRAW_ARB);
  delete [] m_verts;
  m_verts = NULL;

  glGenBuffersARB(1, &m_vb_texA);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texA);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * 2 * 4 * sizeof(float), m_texA, GL_STATIC_DRAW_ARB);
  delete [] m_texA;
  m_texA = NULL;

  glGenBuffersARB(1, &m_vb_texB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texB);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * 2 * 4 * sizeof(float), m_texB, GL_STATIC_DRAW_ARB);
  delete [] m_texB;
  m_texB = NULL;


//:return;
//  glGenBuffersARB(1, &vb_texC);
//  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vb_texC);
//  glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * 2 * 4 * sizeof(float), texC, GL_STATIC_DRAW_ARB);
//  delete [] texC;
//  texC = NULL;

}

void SkyDome::render(float radius, int levels, int segments) {
  static int done = 0;
  static int size = segments * levels;
  static int counter = 0;
  if (!done)	 {
    domeInit(radius , levels, segments);
    done = 1;
  }

  glColor3f(1.0f, 1.0f, 1.0f);
  ++counter;
  counter = counter % 24000;
 
 float val = (float)(counter) / 24000.0f;

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_verts);
  glVertexPointer(3, GL_FLOAT, 0, m_verts);

  RenderSystem::getInstance().switchTexture(m_textures[0]);
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
    glLoadIdentity();
    glTranslatef(val , 0.0f,0.0f);
  glMatrixMode(GL_MODELVIEW);

glBegin(GL_QUADS);
glTexCoord2i(0,0);
glVertex3f(-1.0f, -1.0f, 1.0f);
glVertex3f(1.0f, -1.0f, 1.0f);
glVertex3f(1.0f, -1.0f, -1.0f);
glVertex3f(-1.0f, -1.0f, -1.0f);

glVertex3f(-1.0f, 1.0f, 1.0f);
glVertex3f(1.0f, 1.0f, 1.0f);
glVertex3f(1.0f, 1.0f, -1.0f);
glVertex3f(-1.0f, 1.0f, -1.0f);

glVertex3f(-1.0f, 1.0f, 1.0f);
glVertex3f(-1.0f, -1.0f, 1.0f);
glVertex3f(-1.0f, -1.0f, -1.0f);
glVertex3f(-1.0f, 1.0f, -1.0f);

glVertex3f(1.0f, 1.0f, 1.0f);
glVertex3f(1.0f, -1.0f, 1.0f);
glVertex3f(1.0f, -1.0f, -1.0f);
glVertex3f(1.0f, 1.0f, -1.0f);
glEnd(); 

// Quick hack so we dont call readpixels every frame
static int delay = 0;
if (++delay == 20) {
GLfloat i[4];
glReadPixels(0,0,1,1,GL_RGBA,GL_FLOAT, &i);
glFogfv(GL_FOG_COLOR,i);
 delay = 0;
}
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texA);
  glTexCoordPointer(2, GL_FLOAT, 0, m_texA);
  
  RenderSystem::getInstance().switchTexture(m_textures[0]);
  glDrawArrays(GL_QUADS, 0, size * 4);

  glEnable(GL_BLEND);
  RenderSystem::getInstance().switchTexture(m_textures[1]);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
#define DIST 100.0f
glBegin(GL_QUADS);
glTexCoord2i(0,0);
glVertex3f(-DIST, DIST, 1.0f);
glTexCoord2i(5,0);
glVertex3f(DIST, DIST, 1.0f);
glTexCoord2i(5,5);
glVertex3f(DIST, -DIST, 1.0f);
glTexCoord2i(0,5);
glVertex3f(-DIST, -DIST, 1.0f);
glEnd();
//  glTexCoordPointer(2, GL_FLOAT, 0, m_texB);
//  glDrawArrays(GL_QUADS, 0, size* 4);
  glMatrixMode(GL_TEXTURE);
    glTranslatef(val , 0.0f,0.0f);
  glMatrixMode(GL_MODELVIEW);

  RenderSystem::getInstance().switchTexture(m_textures[2]);
glBegin(GL_QUADS);
glTexCoord2i(0,0);
glVertex3f(-DIST, DIST, 1.0f);
glTexCoord2i(5,0);
glVertex3f(DIST, DIST, 1.0f);
glTexCoord2i(5,5);
glVertex3f(DIST, -DIST, 1.0f);
glTexCoord2i(0,5);
glVertex3f(-DIST, -DIST, 1.0f);
glEnd();

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glDisable(GL_BLEND);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}




}
