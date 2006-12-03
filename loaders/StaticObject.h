// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#ifndef SEAR_RENDERERS_STATICOBJECT_H
#define SEAR_RENDERERS_STATICOBJECT_H 1

#include <vector>

#include <sage/GL.h>
#include "common/Matrix.h"

namespace Sear {

class WorldEntity;

class StaticObject {//: public CacheObject {
public:
  StaticObject();
  ~StaticObject();

  int init();
  void shutdown();

  bool isInitialised() const { return m_initialised; }

  void render(bool select_mode) const;
  void render(bool select_mode, const std::list<std::pair<Matrix, WorldEntity*> > &positions) const;

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

  void copyVertexData(float *ptr, size_t size) {
    if (m_vertex_data) delete [] m_vertex_data;
    m_vertex_data = new float[size];
    memcpy(m_vertex_data, ptr, size * sizeof(float));
  }

  void copyNormalData(float *ptr, size_t size) {
    if (m_normal_data) delete [] m_normal_data;
    m_normal_data = new float[size];
    memcpy(m_normal_data, ptr, size * sizeof(float));
  }

  void copyTextureData(float *ptr, size_t size) {
    if (m_texture_data) delete [] m_texture_data;
    m_texture_data = new float[size];
    memcpy(m_texture_data, ptr, size * sizeof(float));
  }

  void copyIndices(int *ptr, size_t size) {
    if (m_indices) delete [] m_indices;
    m_indices = new int[size];
    memcpy(m_indices, ptr, size * sizeof(int));
  }

  float *createVertexData(size_t size) {
    if (m_vertex_data) delete [] m_vertex_data;
    m_vertex_data = new float[size];
    return m_vertex_data;
  }

  float *createNormalData(size_t size) {
    if (m_normal_data) delete [] m_normal_data;
    m_normal_data = new float[size];
    return m_normal_data;
  }

  float *createTextureData(size_t size) {
    if (m_texture_data) delete [] m_texture_data;
    m_texture_data = new float[size];
    return m_texture_data;
  }

  int *createIndices(size_t size) {
    if (m_indices) delete [] m_indices;
    m_indices = new int[size];
    return m_indices;
  }

  void setNumPoints(unsigned int n) { m_num_points = n; }
  unsigned int getNumPoints() const { return m_num_points; }

  void setNumFaces(unsigned int n) { m_num_faces = n; }
  unsigned int getNumFaces() const { return m_num_faces; }

  float *getVertexDataPtr() { return m_vertex_data; }
  float *getNormalDataPtr() { return m_normal_data; }
  float *getTextureDataPtr() { return m_texture_data; }
  int *getIndicesPtr() { return m_indices; }

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

  void setState(int s) { m_state = s; }
  int getState() const { return m_state; }
 
  void setSelectState(int s) { m_select_state = s; }
  int getSelectState() const { return m_select_state; }

//  void setType(GLenum type) { m_type = type; } 

  Matrix &getMatrix() { return m_matrix; }
  const Matrix &getMatrix() const { return m_matrix; }

  Matrix &getTexMatrix() { return m_tex_matrix; }
  const Matrix &getTexMatrix() const { return m_tex_matrix; }

//  int getType() { return 1; }
  StaticObject *newInstance() { return new StaticObject(); }
  int load(const std::string &filename);
  int save(const std::string &filename);
 
private:
  void createVBOs() const;

  bool m_initialised;

  float *m_vertex_data;
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
  int m_state, m_select_state; 
 

  // Making this stuff mutable may well be bad practise. They are lazily 
  // assigned during the first pass of the render function after a
  // contextCreated call. Perhaps the contextCreated call should really do the
  // allocation.....
  mutable GLuint m_vb_vertex_data, m_vb_normal_data, m_vb_texture_data, m_vb_indices;
  mutable GLuint m_disp_list, m_select_disp_list;
  mutable int m_list_count;

  Matrix m_matrix;
  Matrix m_tex_matrix;

  int m_context_no;
};

} // namespace Sear

#endif // SEAR_RENDERERS_STATIC_OBJECT_H
