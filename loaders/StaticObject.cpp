// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

#include <cassert>

#include <sage/sage.h>
#include <sage/GL.h>

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "src/WorldEntity.h"

#include  "StaticObject.h"


static GLfloat halo_colour[4] = {1.0f, 0.0f, 1.0f, 1.0f};
static GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };

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
  m_state(0),
  m_select_state(0),
  m_vb_vertex_data(0),
  m_vb_normal_data(0),
  m_vb_texture_data(0),
  m_vb_indices(0),
  m_disp_list(0),
  m_select_disp_list(0),
  m_list_count(0),
  m_context_no(-1)
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
void StaticObject::createVBOs() const {
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

int StaticObject::contextCreated() {
  assert(RenderSystem::getInstance().getRenderer()->contextValid());
  // We could have contextCreated called several times for a shared mesh
  assert(m_context_no == -1 || m_context_no == RenderSystem::getInstance().getRenderer()->currentContextNo());
  m_context_no = RenderSystem::getInstance().getRenderer()->currentContextNo();
  return 0;
}

void StaticObject::contextDestroyed(bool check) {
  assert(m_initialised == true);

  // We could have contextDestroyed called several times for a shared mesh
  if (m_context_no == -1) return;

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
    if (glIsList(m_select_disp_list)) glDeleteLists(m_list_count, m_select_disp_list);
    if (glIsList(m_disp_list)) glDeleteLists(m_list_count, m_disp_list);
  }
  m_list_count = 0;
  m_vb_vertex_data = 0;
  m_vb_normal_data = 0;
  m_vb_texture_data = 0;
  m_vb_indices = 0;
  m_select_disp_list = 0;
  m_disp_list = 0;

  m_context_no = -1;
}

void StaticObject::render(bool select_mode) const {
  assert(m_initialised == true);
  assert(RenderSystem::getInstance().getRenderer()->contextValid());
  assert(m_context_no == RenderSystem::getInstance().getRenderer()->currentContextNo());
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

      m_list_count = 1;
      disp = glGenLists(m_list_count);
      glNewList(disp, GL_COMPILE);

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
        glLockArraysEXT(0, m_num_points);
      }

      if (m_indices) {
        glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
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
      glCallList(disp);
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

void StaticObject::render(bool select_mode, const std::list<std::pair<Matrix, WorldEntity*> > &positions) const {
  assert(m_initialised == true);
  assert(RenderSystem::getInstance().getRenderer()->contextValid());
  assert(m_context_no == RenderSystem::getInstance().getRenderer()->currentContextNo());

  // Setup texture transform
  float m[4][4];
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
    }

    /// Loop through each matrix here and do the render
    std::list<std::pair<Matrix,WorldEntity*> >::const_iterator I = positions.begin();
    while (I != positions.end()) {
      const Matrix &mx = (*I).first;
      WorldEntity *we = (*I).second;

      if (select_mode) {
        RenderSystem::getInstance().getRenderer()->nextColour(we);
      }

      glPushMatrix();
      // Set transform
      // Apply position transform 
      mx.getMatrix(m);
      glMultMatrixf(&m[0][0]);
      // Apply mesh transform
      m_matrix.getMatrix(m);
      glMultMatrixf(&m[0][0]);

      // Do we need to highlight this mesh?
      if (!select_mode && we->isSelectedEntity()) {
        if (m_use_stencil) { // Can we use the stencil buffer?
          glEnable(GL_STENCIL_TEST);
          glStencilFunc(GL_ALWAYS, -1, 1);
          glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

          if (m_indices) {
            glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
          } else  {
            glDrawArrays(GL_TRIANGLES, 0, m_num_points);
          }

          RenderSystem::getInstance().switchState(m_select_state);
          glStencilFunc(GL_NOTEQUAL, -1, 1);
          glColor4fv(halo_colour);
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

          if (m_indices) {
             glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
          } else  {
            glDrawArrays(GL_TRIANGLES, 0, m_num_points);
          }

          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          glDisable(GL_STENCIL_TEST);
        } else { // Just render object solid highligh colour
          RenderSystem::getInstance().switchState(m_select_state);
          glColor4fv(halo_colour);
  
          if (m_indices) {
            glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
          } else  {
            glDrawArrays(GL_TRIANGLES, 0, m_num_points);
          }
        }
        glColor4fv(white);
        RenderSystem::getInstance().switchState(m_state);
      } else { // Render object normally
        if (m_indices) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }
      }

      glPopMatrix();
      ++I;
    }
    /// end of loop

    // Reset opengl state
    if (m_indices) {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
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
  } else { // Fall back to vertex arrays and display lists

    GLuint &disp = (select_mode) ? (m_select_disp_list) : (m_disp_list);
    if (!glIsList(disp)) {
      // Need to reset textures otherwise the display list may not record a
      // texture change if the previous object has the same texture. This is 
      // fine until the order of objects changes and the wrong texture is in 
      // place.
      for (unsigned int i = 0; i < m_textures.size(); ++i) {
        RenderSystem::getInstance().switchTexture(i, 0);
      }
      // Reset current texture unit
      glActiveTextureARB(GL_TEXTURE0_ARB);

      m_list_count = 6;
      disp = glGenLists(m_list_count);

      // First display list, set up state
      glNewList(disp, GL_COMPILE);

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
        glLockArraysEXT(0, m_num_points);
      }
      glEndList();

 
      // Third disp list, render the object
      glNewList(disp + 2, GL_COMPILE);
      if (m_indices) {
        glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
      } else  {
        glDrawArrays(GL_TRIANGLES, 0, m_num_points);
      }
      glEndList();

      if (m_use_stencil) {      
      // Second display list, setup stencil op for outlines
      glNewList(disp + 1, GL_COMPILE);
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, -1, 1);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      glEndList();
   
      // Fourth list, render the outline
      glNewList(disp + 3, GL_COMPILE);
      RenderSystem::getInstance().forceState(m_select_state);
      glStencilFunc(GL_NOTEQUAL, -1, 1);
      glColor4fv(halo_colour);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glEndList();


      //Fifth list, finish outline
      glNewList(disp + 4, GL_COMPILE);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDisable(GL_STENCIL_TEST);
      glColor4fv(white);
      RenderSystem::getInstance().forceState(m_state);
      glEndList();
      } else {
      // Fourth list, render the outline
      glNewList(disp + 3, GL_COMPILE);
      RenderSystem::getInstance().forceState(m_select_state);
      glColor4fv(halo_colour);
      glEndList();


      //Fifth list, finish outline
      glNewList(disp + 4, GL_COMPILE);
      glColor4fv(white);
      RenderSystem::getInstance().forceState(m_state);
      glEndList();
 

      }
      // sixth disp list, clean up state 
      glNewList(disp + 5, GL_COMPILE);
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

    // Setup initial state
    glCallList(disp);

    /// Loop through each matrix here and do the render
    std::list<std::pair<Matrix, WorldEntity*> >::const_iterator I = positions.begin();
    while (I != positions.end()) {
      const Matrix &mx = (*I).first;
      WorldEntity *we = (*I).second;

      assert(we != 0);

      if (select_mode) {
        RenderSystem::getInstance().getRenderer()->nextColour(we);
      }

      glPushMatrix();
      // Set transform
      // Apply position transform 
      mx.getMatrix(m);
      glMultMatrixf(&m[0][0]);
      // Apply mesh transform
      m_matrix.getMatrix(m);
      glMultMatrixf(&m[0][0]);

      // Render stuff
      if (!select_mode && we->isSelectedEntity()) {
        if (m_use_stencil) {
          glCallList(disp + 1);
          glCallList(disp + 2);
          glCallList(disp + 3);
          glCallList(disp + 2);
          glCallList(disp + 4);
        } else {
          glCallList(disp + 3);
          glCallList(disp + 2);
          glCallList(disp + 4);
        }
      } else {
        glCallList(disp + 2);
      }

      glPopMatrix();
      ++I;
    }
    // Reset state
    glCallList(disp + 5);
  }

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
 
}

} // namespace Sear
