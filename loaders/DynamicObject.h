// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

#ifndef SEAR_RENDERERS_DYNAMICOBJECT_H
#define SEAR_RENDERERS_DYNAMICOBJECT_H 1

#include <vector>

#include <sage/GL.h>

//#include "src/CacheObject.h"
#include "common/Matrix.h"

namespace Sear {

class DynamicObject {//: public CacheObject {
public:
  DynamicObject();
  ~DynamicObject();

  int init();
  void shutdown();

  bool isInitialised() const { return m_initialised; }

  void render(bool select_mode) const;

  int contextCreated();
  void contextDestroyed(bool check);

  void setTexture(unsigned int num, int texture, int texture_mask) {
    if (m_textures.size() <= num) {
      m_textures.resize(num + 1);
      m_texture_masks.resize(num + 1);
    }
    m_textures[num] = texture;
    m_texture_masks[num] = texture_mask;
  }

  int getTexture(unsigned int num, int &texture, int &texture_mask) {
    if (num >= m_textures.size()) return 1;
    texture      = m_textures[num];
    texture_mask = m_texture_masks[num];
    return 0;
  }

  void copyVertexData(float *ptr, size_t size);
  void copyColourData(unsigned char *ptr, size_t size);
  void copyNormalData(float *ptr, size_t size);
  void copyTextureData(float *ptr, size_t size);
  void copyIndices(int *ptr, size_t size);

  float *createVertexData(int size);
  unsigned char *createColourData(int size);
  float *createNormalData(int size);
  float *createTextureData(int size);
  int *createIndices(int size);

  void setNumPoints(int n) { m_num_points = n; }
  int getNumPoints() const { return m_num_points; }

  void setNumFaces(unsigned int n) { m_num_faces = n; }
  unsigned int getNumFaces() const { return m_num_faces; }

  float *getVertexDataPtr();
  unsigned char *getColourDataPtr();
  float *getNormalDataPtr();
  float *getTextureDataPtr();
  int *getIndicesPtr();

  void releaseVertexDataPtr();
  void releaseColourDataPtr();
  void releaseNormalDataPtr();
  void releaseTextureDataPtr();
  void releaseIndicesPtr();

  void setAmbient(float a[4]) {
    m_ambient[0] = a[0];
    m_ambient[1] = a[1];
    m_ambient[2] = a[2];
    m_ambient[3] = a[3];
  }

  void setDiffuse(float d[4]) {
    m_diffuse[0] = d[0];
    m_diffuse[1] = d[1];
    m_diffuse[2] = d[2];
    m_diffuse[3] = d[3];
  }

  void setSpecular(float s[4]) {
    m_specular[0] = s[0];
    m_specular[1] = s[1];
    m_specular[2] = s[2];;
    m_specular[3] = s[3];
  }
 
  void setEmission(float e[4]) {
    m_emission[0] = e[0];
    m_emission[1] = e[1];
    m_emission[2] = e[2];
    m_emission[3] = e[3];
  }


  void setAmbient(float r, float g, float b, float a) {
    m_ambient[0] = r;
    m_ambient[1] = g;
    m_ambient[2] = b;
    m_ambient[3] = a;
  }

  void setDiffuse(float r, float g, float b, float a) {
    m_diffuse[0] = r;
    m_diffuse[1] = g;
    m_diffuse[2] = b;
    m_diffuse[3] = a;
  }

  void setSpecular(float r, float g, float b, float a) {
    m_specular[0] = r;
    m_specular[1] = g;
    m_specular[2] = b;
    m_specular[3] = a;
  }
 
  void setEmission(float r, float g, float b, float a) {
    m_emission[0] = r;
    m_emission[1] = g;
    m_emission[2] = b;
    m_emission[3] = a;
  }

  void setShininess(float s) { m_shininess = s; }

//  void setState(int s) { m_state = s; }
//  int getState() const { return m_state; }
 
//  void setType(GLenum type) { m_type = type; } 
/*
  void setMatrix(float **m) {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        m_matrix[i][j] = m[i][j];
      }
    }
  }
*/
  Matrix &getMatrix() { return m_matrix; }
  const Matrix &getMatrix() const { return m_matrix; }


/*  void identity() {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (i == j)  m_matrix[i][j] = 1.0f;
        else  m_matrix[i][j] = 0.0f;
      }
    }
  }

  void scale(float s) {
    for (int i = 0; i < 4; ++i) {
      m_matrix[i][i] *= s;
    }
  }

  void translate(float x, float y, float z) {
    m_matrix[0][3] += x;
    m_matrix[1][3] += y;
    m_matrix[2][3] += z;
  }

*/
//  int getType() { return 1; }
  DynamicObject *newInstance() { return new DynamicObject(); }
 
private:
  void createVBOs();

  bool m_initialised;

  float *m_vertex_data;
  unsigned char *m_colour_data;
  float *m_normal_data;
  float *m_texture_data;
  int *m_indices;

  unsigned int m_num_points;
  unsigned int m_num_faces;
  std::vector<int> m_textures;
  std::vector<int> m_texture_masks;

  float m_ambient[4];
  float m_diffuse[4];
  float m_specular[4];
  float m_emission[4];
  float m_shininess;

  // I.e. GL_TRIANGLES, GL_QUADS etc..
//  GLenum m_type;
//  int m_state; 
  
  mutable GLuint m_vb_vertex_data, m_vb_colour_data, m_vb_normal_data, m_vb_texture_data, m_vb_indices;

  //float m_matrix[4][4];
  Matrix m_matrix;
  int m_context_no;
};

} // namespace Sear

#endif // SEAR_RENDERERS_DYNAMICOBJECT_H
