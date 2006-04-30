// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#include <cassert>

#include <sage/sage.h>
#include <sage/GL.h>

#include "renderers/RenderSystem.h"

#include  "StaticObject.h"

namespace Sear {

StaticObject::StaticObject() :
  m_initialised(false),
  m_vertex_data(0),
  m_normal_data(0),
  m_texture_data(0),
  m_indices(0),
  m_num_points(0),
  m_num_faces(0),
//  m_type(0),
//  m_state(0),
  m_vb_vertex_data(0),
  m_vb_normal_data(0),
  m_vb_texture_data(0),
  m_vb_indices(0),
  m_disp_list(0),
  m_select_disp_list(0)
{
}

StaticObject::~StaticObject()  {
  assert(m_initialised == false);
}

int StaticObject::init() {
  assert(m_initialised == false);

  setAmbient(1.0f, 1.0f, 1.0f, 1.0f);
  setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
  setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
  setEmission(0.0f, 0.0f, 0.0f, 0.0f);
  setShininess(50.0f);

  m_matrix.identity();
  m_tex_matrix.identity();

  m_initialised = true;

  return 0;
}

void StaticObject::shutdown()  {
  assert(m_initialised == true);
  contextDestroyed(true);

  if (m_vertex_data) delete [] m_vertex_data;
  if (m_normal_data) delete [] m_normal_data;
  if (m_texture_data) delete [] m_texture_data;
  if (m_indices) delete [] m_indices;

  m_initialised = false;

}
void StaticObject::createVBOs() {
  assert(m_initialised == true);
  assert(sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT] == true);

  // Generate vertex array vbo  
  glGenBuffersARB(1, &m_vb_vertex_data);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_vertex_data);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_points * 3 * sizeof(float), m_vertex_data, GL_STATIC_DRAW_ARB);

  // Generate normal vbo
  if (m_normal_data) {
    glGenBuffersARB(1, &m_vb_normal_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_normal_data);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_points * 3 * sizeof(float), m_normal_data, GL_STATIC_DRAW_ARB);
  }

  // Generate texture coord vbo
  if (m_texture_data) {
    glGenBuffersARB(1, &m_vb_texture_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texture_data);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_points * 2 * sizeof(float), m_texture_data, GL_STATIC_DRAW_ARB);
  }
  // Reset VBO buffer
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  if (m_indices) {
    // Bind indices
    glGenBuffersARB(1, &m_vb_indices);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vb_indices);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_num_faces * 3 * sizeof(int), m_indices, GL_STATIC_DRAW_ARB);
    // Reset VBO buffer
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
}

void StaticObject::contextDestroyed(bool check) {
  assert(m_initialised == true);
  if (check) {
    // Clean up vertex buffer objects
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      if (glIsBufferARB(m_vb_vertex_data)) {
        glDeleteBuffersARB(1, &m_vb_vertex_data);
      }
      if (glIsBufferARB(m_vb_normal_data)) {
        glDeleteBuffersARB(1, &m_vb_normal_data);
      }
      if (glIsBufferARB(m_vb_texture_data)) {
       glDeleteBuffersARB(1, &m_vb_texture_data);
      }
      if (glIsBufferARB(m_vb_indices)) {
       glDeleteBuffersARB(1, &m_vb_indices);
      }
    }

    // Clean up display lists 
    if (glIsList(m_select_disp_list)) glDeleteLists(1, m_select_disp_list);
    if (glIsList(m_disp_list)) glDeleteLists(1, m_disp_list);
  }
  m_vb_vertex_data = 0;
  m_vb_normal_data = 0;
  m_vb_texture_data = 0;
  m_vb_indices = 0;
  m_select_disp_list = 0;
  m_disp_list = 0;
}

void StaticObject::render(bool select_mode) {
  assert(m_initialised == true);
  glPushMatrix();
  // Set transform
  float m[4][4];
  m_matrix.getMatrix(m);
  glMultMatrixf(&m[0][0]);

  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  m_tex_matrix.getMatrix(m);
  glMultMatrixf(&m[0][0]);
  glMatrixMode(GL_MODELVIEW);
    

   // If VBO's are enabled
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_vertex_data)) createVBOs();

    // Set material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   m_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   m_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  m_emission);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);

    // Bind vertex array
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_vertex_data);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    // Bind normal array
    if (glIsBufferARB(m_vb_normal_data)) {
      glEnableClientState(GL_NORMAL_ARRAY);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_normal_data);
      glNormalPointer(GL_FLOAT, 0, 0);
    }

    if (glIsBufferARB(m_vb_texture_data)) {
      for (unsigned int i = 0; i < m_textures.size(); ++i) {
        // Bind texture array
        glActiveTextureARB(GL_TEXTURE0_ARB + i);
        if (select_mode) {
          RenderSystem::getInstance().switchTexture(m_texture_masks[i]);
        } else {
          RenderSystem::getInstance().switchTexture(m_textures[i]); 
        }
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texture_data);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
      }
      // Reset current texture unit
      glActiveTextureARB(GL_TEXTURE0_ARB);
    }

    if (m_indices) {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vb_indices);
      glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    } else  {
      glDrawArrays(GL_TRIANGLES, 0, m_num_points);
    }
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    if (glIsBufferARB(m_vb_texture_data)) {
      for (unsigned int i = 0; i < m_textures.size(); ++i) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      }
      // Reset current texture unit
      glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    if (glIsBufferARB(m_vb_normal_data)) {
      glDisableClientState(GL_NORMAL_ARRAY);
    }
  } else {
    GLuint &disp = (select_mode) ? (m_select_disp_list) : (m_disp_list);
    if (glIsList(disp)) {
      glCallList(disp);
    } else {
      // Need to reset textures otherwise the display list may not record a
      // texture change if the previous object has the same texture. This is 
      // fine until the order of objects changes and the wrong texture is in 
      // place.
      for (unsigned int i = 0; i < m_textures.size(); ++i) {
        RenderSystem::getInstance().switchTexture(i, 0);
      }
      // Reset current texture unit
      glActiveTextureARB(GL_TEXTURE0_ARB);

      disp = glGenLists(1);
      glNewList(disp, GL_COMPILE_AND_EXECUTE);
      // Setup client states
      glEnableClientState(GL_NORMAL_ARRAY);

      // Set material properties
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   m_ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   m_diffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m_specular);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  m_emission);
      glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);

      // Bind vertex array
      glVertexPointer(3, GL_FLOAT, 0, m_vertex_data);

      // Bind normal array
      if (m_normal_data) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, m_normal_data);
      }

      if (m_texture_data) {
        for (unsigned int i = 0; i < m_textures.size(); ++i) {
          // Bind texture array
          glActiveTextureARB(GL_TEXTURE0_ARB + i);
          if (select_mode) {
            RenderSystem::getInstance().switchTexture(m_texture_masks[i]);
          } else {
            RenderSystem::getInstance().switchTexture(m_textures[i]); 
          }
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glTexCoordPointer(2, GL_FLOAT, 0, m_texture_data);
        }

        // Reset current texture unit
        glActiveTextureARB(GL_TEXTURE0_ARB);
      }


      // Use the Lock arrays extension if available.
      if (sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY]) {
        glLockArraysEXT(0, m_num_points  * 3);
      }

      if (m_indices) {
        glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
      } else  {
        glDrawArrays(GL_TRIANGLES, 0, m_num_points);
      }

      if (sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY]) {
        glUnlockArraysEXT();
      }

      if (m_texture_data) {
        for (unsigned int i = 0; i < m_textures.size(); ++i) {
          glActiveTextureARB(GL_TEXTURE0 + i);
          glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
  
        // Reset current texture unit
        glActiveTextureARB(GL_TEXTURE0_ARB);
      }

      if (m_normal_data) {
        glDisableClientState(GL_NORMAL_ARRAY);
      }
      glEndList();
    }
  }
  glPopMatrix();

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
 
}

int StaticObject::load(const std::string &filename) {
#if(0)
  assert(m_initialised == true);
  FILE *fp = fopen(filename.c_str(), "rb");

  if (!fp) {
    fprintf(stderr, "Error opening %s for reading.\n", filename.c_str());
    return 1;
  }

  char buf[8];
  size_t num = fread(&buf[0], sizeof(char), 8, fp);

  if (num != 8) {
    fprintf(stderr, "Error reading header (%s).\n", filename.c_str());
    fclose(fp);
    return -1;
  }

  // Check header is as expected
  if (!strncmp(buf, "CACHEOBJ", 8)) {
    fprintf(stderr, "File is not a cache object (%s).\n", filename.c_str());
    fclose(fp);
    return -1;
  }

  // Read ID for Cache Object type
  int id;
  num = fread(&id, sizeof(int), 1, fp);
  if (num != 1) {
    fprintf(stderr, "Error reading ID.\n");
    fclose(fp);
    return -1;
  }
  
  if (id != getType()) {
    fprintf(stderr, "Error CacheObject type %d is not type %d .\n", id, getType());
    return 1;
  }

  // Get number of points
  num = fread(&m_num_points, sizeof(int), 1, fp);
  if (num != 1) goto error;

  // Read in vertex data
  num = fread(&m_vertex_data, sizeof(float), m_num_points * 3, fp);
  if (num != (size_t)m_num_points * 3) goto error;
  
  // Read in normal data
  num = fread(&m_normal_data, sizeof(float), m_num_points * 3, fp);
  if (num != (size_t)m_num_points * 3) goto error;

  // Read in texture data
  num = fread(&m_texture_data, sizeof(float), m_num_points * 2, fp);
  if (num != (size_t)m_num_points * 2) goto error;

  // Read in indices
  num = fread(&m_indices, sizeof(int), m_num_points, fp);
  if (num != (size_t)m_num_points) goto error;
  // Read in material properties  
  num = fread(&m_ambient, sizeof(float), 4, fp);
  if (num != 4) goto error;
  num = fread(&m_diffuse, sizeof(float), 4, fp);
  if (num != 4) goto error;
  num = fread(&m_specular, sizeof(float), 4, fp);
  if (num != 4) goto error;
  num = fread(&m_emission, sizeof(float), 4, fp);
  if (num != 4) goto error;
  num = fread(&m_shininess, sizeof(float), 1, fp);
  if (num != 1) goto error;

  num = fread(&m_type, sizeof(GLenum), 1, fp);
  if (num != 1) goto error;

  num = fread(&m_state, sizeof(int), 1, fp);
  if (num != 1) goto error;

  fclose(fp); 
  return 0;

error:
  fclose (fp);
  fprintf(stderr, "Error reading StaticObject.\n");
#endif
  return 1;

}

int StaticObject::save(const std::string &filename) {
  assert(m_initialised == true);
  return 0;
}

} // namespace Sear
