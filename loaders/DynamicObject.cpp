// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

#include <cassert>

#include <sage/sage.h>
#include <sage/GL.h>

#include "renderers/RenderSystem.h"

#include "src/WorldEntity.h"

#include  "DynamicObject.h"


static GLfloat halo_colour[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };

namespace Sear {

DynamicObject::DynamicObject() :
  m_initialised(false),
  m_vertex_data(NULL),
  m_colour_data(NULL),
  m_normal_data(NULL),
  m_texture_data(NULL),
  m_indices(NULL),
  m_num_points(0),
  m_num_faces(0),
  m_state(-1),
  m_select_state(-1),
  m_vb_vertex_data(0),
  m_vb_colour_data(0),
  m_vb_normal_data(0),
  m_vb_texture_data(0),
  m_vb_indices(0),
  m_context_no(-1),
  m_use_stencil(false)
{
}

DynamicObject::~DynamicObject() {
  if (m_initialised) shutdown();
}

int DynamicObject::init() {
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

void DynamicObject::shutdown()  {
  assert(m_initialised == true);
  contextDestroyed(true);

  if (m_vertex_data) delete [] m_vertex_data;
  if (m_colour_data) delete [] m_colour_data;
  if (m_normal_data) delete [] m_normal_data;
  if (m_texture_data) delete [] m_texture_data;
  if (m_indices) delete [] m_indices;

  m_initialised = false;

}

void DynamicObject::copyVertexData(float *ptr, size_t size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_vertex_data)) glGenBuffersARB(1, &m_vb_vertex_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_vertex_data);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(float), ptr, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  } else {
    if (m_vertex_data) delete [] m_vertex_data;
    m_vertex_data = new float[size];
    memcpy(m_vertex_data, ptr, size * sizeof(float));
  }
}

void DynamicObject::copyColourData(unsigned char *ptr, size_t size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_colour_data)) glGenBuffersARB(1, &m_vb_colour_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_colour_data);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size  * sizeof(unsigned char), ptr, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  } else {
    if (m_colour_data) delete [] m_colour_data;
    m_colour_data = new unsigned char[size];
    memcpy(m_colour_data, ptr, size * sizeof(unsigned char));
  }
}

void DynamicObject::copyNormalData(float *ptr, size_t size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_normal_data)) glGenBuffersARB(1, &m_vb_normal_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_normal_data);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(float), ptr, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  } else {
    if (m_normal_data) delete [] m_normal_data;
    m_normal_data = new float[size];
    memcpy(m_normal_data, ptr, size * sizeof(float));
  }
}
void DynamicObject::copyTextureData(float *ptr, size_t size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_texture_data)) glGenBuffersARB(1, &m_vb_texture_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texture_data);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(float), ptr, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  } else {
    if (m_texture_data) delete [] m_texture_data;
    m_texture_data = new float[size];
    memcpy(m_texture_data, ptr, size * sizeof(float));
  }
}

void DynamicObject::copyIndices(int *ptr, size_t size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_indices)) glGenBuffersARB(1, &m_vb_indices);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_vb_indices);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(int), ptr, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
  } else {
    if (m_indices) delete [] m_indices;
    m_indices = new int[size];
    memcpy(m_indices, ptr, size * sizeof(int));
  }
}

int DynamicObject::contextCreated() {
  assert(RenderSystem::getInstance().contextValid());
  // We could have contextCreated called several times for a shared mesh
  assert(m_context_no == -1 || m_context_no == RenderSystem::getInstance().currentContextNo());
  m_context_no = RenderSystem::getInstance().currentContextNo();
  return 0;
}

void DynamicObject::contextDestroyed(bool check) {
  assert(m_initialised == true);

  // We could have contextDestroyed called several times for a shared mesh
  if (m_context_no == -1) return;

  if (check) {
    // Clean up vertex buffer objects
    if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
      if (glIsBufferARB(m_vb_vertex_data)) {
        glDeleteBuffersARB(1, &m_vb_vertex_data);
      }
      if (glIsBufferARB(m_vb_colour_data)) {
        glDeleteBuffersARB(1, &m_vb_colour_data);
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
  }
  m_vb_vertex_data = 0;
  m_vb_colour_data = 0;
  m_vb_normal_data = 0;
  m_vb_texture_data = 0;
  m_vb_indices = 0;

  m_context_no = -1;
}

void DynamicObject::render(bool select_mode) const {
  assert(m_initialised == true);

  assert(RenderSystem::getInstance().contextValid());
  assert(m_context_no == RenderSystem::getInstance().currentContextNo());

  glPushMatrix();

  // Set transform
  glMultMatrixf(m_matrix.getMatrix());

   // If VBO's are enabled
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    // Setup client states
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

    if (glIsBufferARB(m_vb_colour_data)) {
      glEnableClientState(GL_COLOR_ARRAY);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_colour_data);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
    }

    if (glIsBufferARB(m_vb_indices)) {
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
      glActiveTextureARB(GL_TEXTURE0);
    }

    if (glIsBufferARB(m_vb_normal_data)) {
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (glIsBufferARB(m_vb_colour_data)) {
      glDisableClientState(GL_COLOR_ARRAY);
    }

  } else {
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

    if (m_colour_data) {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_colour_data);
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

    if (m_colour_data) {
      glDisableClientState(GL_COLOR_ARRAY);
    }

    if (m_normal_data) {
      glDisableClientState(GL_NORMAL_ARRAY);
    }
  }
  glPopMatrix();
}

float *DynamicObject::createVertexData(int size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_vertex_data)) glGenBuffersARB(1, &m_vb_vertex_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_vertex_data);

    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(float), 0, GL_STREAM_DRAW_ARB);
    return (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    if (m_vertex_data) delete [] m_vertex_data;
    m_vertex_data = new float[size];
    return m_vertex_data;
  }
}

unsigned char *DynamicObject::createColourData(int size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_colour_data)) glGenBuffersARB(1, &m_vb_colour_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_colour_data);

    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(unsigned char), 0, GL_STREAM_DRAW_ARB);
    return (unsigned char*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    if (m_colour_data) delete [] m_colour_data;
    m_colour_data = new unsigned char[size];
    return m_colour_data;
  }
}

float *DynamicObject::createNormalData(int size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_normal_data)) glGenBuffersARB(1, &m_vb_normal_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_normal_data);

    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(float), 0, GL_STREAM_DRAW_ARB);
    return (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    if (m_normal_data) delete [] m_normal_data;
    m_normal_data = new float[size];
    return m_normal_data;
  }
}

float *DynamicObject::createTextureData(int size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_texture_data)) glGenBuffersARB(1, &m_vb_texture_data);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texture_data);

    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * sizeof(float), 0, GL_STREAM_DRAW_ARB);
    return (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    if (m_texture_data) delete [] m_texture_data;
    m_texture_data = new float[size];
    return m_texture_data;
  }
}

int *DynamicObject::createIndices(int size) {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    if (!glIsBufferARB(m_vb_indices)) glGenBuffersARB(1, &m_vb_indices);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vb_indices);

    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size * sizeof(int), 0, GL_STREAM_DRAW_ARB);
    return (int*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    if (m_indices) delete [] m_indices;
    m_indices = new int[size];
    return m_indices;
  }
}

float *DynamicObject::getVertexDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_vertex_data);
    return (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    return m_vertex_data;
  }
}

unsigned char *DynamicObject::getColourDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_colour_data);
    return (unsigned char*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    return m_colour_data;
  }
}

float *DynamicObject::getNormalDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_normal_data);
    return (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    return m_normal_data;
  }
}

float *DynamicObject::getTextureDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vb_texture_data);
    return (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    return m_texture_data;
  }
}

int *DynamicObject::getIndicesPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vb_indices);
    return (int*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
  } else {
    return m_indices;
  }
}

void DynamicObject::releaseVertexDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void DynamicObject::releaseColourDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void DynamicObject::releaseNormalDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void DynamicObject::releaseTextureDataPtr() {
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }
}

void DynamicObject::releaseIndicesPtr() {
 if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
}

/////////////////////////////////////

void DynamicObject::render(bool select_mode, WorldEntity *we) const {
  assert(m_initialised == true);
  assert(RenderSystem::getInstance().contextValid());
  assert(m_context_no == RenderSystem::getInstance().currentContextNo());

  // Setup texture transform
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glMultMatrixf(m_tex_matrix.getMatrix());
  glMatrixMode(GL_MODELVIEW);

   // If VBO's are enabled
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {

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

    if (glIsBufferARB(m_vb_indices)) {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vb_indices);
    }

    if (select_mode) {
      RenderSystem::getInstance().nextColour(we);
    }

    glPushMatrix();
    // Apply mesh transform
    glMultMatrixf(m_matrix.getMatrix());

    // Do we need to highlight this mesh?
    if (!select_mode && we->isSelectedEntity()) {
      if (m_use_stencil) { // Can we use the stencil buffer?
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, -1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        if (glIsBufferARB(m_vb_indices)) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }

        RenderSystem::getInstance().switchState(m_select_state);
        glStencilFunc(GL_NOTEQUAL, -1, 1);
        glColor4fv(halo_colour);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (glIsBufferARB(m_vb_indices)) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_STENCIL_TEST);
      } else { // Just render object solid highligh colour
        RenderSystem::getInstance().switchState(m_select_state);
        glColor4fv(halo_colour);

        if (glIsBufferARB(m_vb_indices)) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }
      }
      glColor4fv(white);
      RenderSystem::getInstance().switchState(m_state);
    } else { // Render object normaly
      GLboolean blend_enabled = true;
      GLboolean cmat_enabled = true;
      bool reset_colour = false;
      if (!select_mode && we->getFade() < 1.0f) {
        glGetBooleanv(GL_BLEND, &blend_enabled);
        glGetBooleanv(GL_COLOR_MATERIAL, &cmat_enabled);
        if (!blend_enabled) glEnable(GL_BLEND);
        // TODO: Why does this appear to be enabled for far too long?
        // Is the state being kept too long, or more likely is the Fade value somehow broken?
        if (!cmat_enabled) glEnable(GL_COLOR_MATERIAL);
        glColor4f(1.0f, 1.0f, 1.0f, we->getFade());
        reset_colour = true;
      }

      if (glIsBufferARB(m_vb_indices)) {
        glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, 0);
      } else  {
        glDrawArrays(GL_TRIANGLES, 0, m_num_points);
      }

      if (!blend_enabled) glDisable(GL_BLEND);
      if (!cmat_enabled)  glDisable(GL_COLOR_MATERIAL);
      if (reset_colour)   glColor4fv(white);
    }

    glPopMatrix();

    // Reset opengl state
    if (glIsBufferARB(m_vb_indices)) {
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
  } else { // Fall back to vertex arrays
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

    if (select_mode) {
      RenderSystem::getInstance().nextColour(we);
    }

    glPushMatrix();
    // Apply mesh transform
    glMultMatrixf(m_matrix.getMatrix());

    // Do we need to highlight this mesh?
    if (!select_mode && we->isSelectedEntity()) {
      if (m_use_stencil) { // Can we use the stencil buffer?
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, -1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        if (m_indices) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }

        RenderSystem::getInstance().switchState(m_select_state);
        glStencilFunc(GL_NOTEQUAL, -1, 1);
        glColor4fv(halo_colour);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (m_indices) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_STENCIL_TEST);
      } else { // Just render object solid highligh colour
        RenderSystem::getInstance().switchState(m_select_state);
        glColor4fv(halo_colour);

        if (m_indices) {
          glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
        } else  {
          glDrawArrays(GL_TRIANGLES, 0, m_num_points);
        }
      }
      glColor4fv(white);
      RenderSystem::getInstance().switchState(m_state);
    } else { // Render object normaly
      GLboolean blend_enabled = true;
      GLboolean cmat_enabled = true;
      bool reset_colour = false;
      if (!select_mode && we->getFade() < 1.0f) {
        glGetBooleanv(GL_BLEND, &blend_enabled);
        glGetBooleanv(GL_COLOR_MATERIAL, &cmat_enabled);
        if (!blend_enabled) glEnable(GL_BLEND);
        // TODO: Why does this appear to be enabled for far too long?
        // Is the state being kept too long, or more likely is the Fade value somehow broken?
        if (!cmat_enabled) glEnable(GL_COLOR_MATERIAL);
        glColor4f(1.0f, 1.0f, 1.0f, we->getFade());
        reset_colour = true;
      }

      if (m_indices) {
        glDrawElements(GL_TRIANGLES, m_num_faces * 3, GL_UNSIGNED_INT, m_indices);
      } else  {
        glDrawArrays(GL_TRIANGLES, 0, m_num_points);
      }

      if (!blend_enabled) glDisable(GL_BLEND);
      if (!cmat_enabled)  glDisable(GL_COLOR_MATERIAL);
      if (reset_colour)   glColor4fv(white);
    }

    glPopMatrix();

    if (m_texture_data) {
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

  }

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
 
}

} // namespace Sear
