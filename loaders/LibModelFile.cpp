// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall

// $Id: LibModelFile.cpp,v 1.13 2005-06-24 08:42:34 simon Exp $

/*
  Debug check list
  * Correct Indices
  * glDrawElements faces pointer data
  * Correct normal calculation

*/

extern "C" {
#include <libmd3/structure.h>
#include <libmd3/loader.h>
#include <libmd3/convert.h>
#include <libmd3/mesh.h>
}

#include <sage/sage.h>
#include <sage/GL.h>

#include "common/Utility.h"

#include "src/System.h"
#include "src/FileHandler.h"

#include "renderers/Render.h"
#include "renderers/RenderSystem.h"

#include "LibModelFile.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

static const float default_scale = 1.0f / 64.0f;

LibModelFile::LibModelFile(Render *render) : Model(render), 
  m_initialised(false),
  m_vertex_data(NULL),
  m_normal_data(NULL),
  m_texel_data(NULL),
  m_num_triangles(0),
  m_num_vertices(0),
  m_render_list(0),
  m_select_list(0)
{
  m_config.sige.connect(SigC::slot(*this, &LibModelFile::varconf_error_callback));
}

LibModelFile::~LibModelFile() {
  assert(m_initialised == false);
}
  
int LibModelFile::init(const std::string &filename) {
  assert(m_initialised == false);

  std::string object;
  if (m_config.readFromFile(filename)) {
    if (m_config.findItem("model", "filename")) {
      object = (std::string)m_config.getItem("model", "filename");
    } else {
      fprintf(stderr, "Error: No md3 filename specified.\n");
      return 1;
    }
  } else {
    fprintf(stderr, "Error reading %s as varconf file. Trying as .md3 file.\n",
            filename.c_str());
    object = filename;
  }

  System::instance()->getFileHandler()->expandString(object);

  // Load 3ds file
  if (debug) printf("LibModelFile: Loading: %s\n", object.c_str());

  libmd3_file *modelFile = libmd3_file_load(object.c_str());
  if (!modelFile) {
    fprintf(stderr, "LibModelFile: Error loading %s file\n", object.c_str());
    return 1;
  }

  for (int i = 0; i < modelFile->header->mesh_count; ++i) {
    libmd3_unpack_normals(&modelFile->meshes[i]);
//    libmd3_strip_env_texcoords(&modelFile->meshes[i]);
  }

  // Create one big array to store all data, then just swap the textures when rendering.

  // Store boundaries in data
  m_boundaries.resize(modelFile->header->mesh_count  + 1);
  m_textures.resize(modelFile->header->mesh_count);
  m_mask_textures.resize(modelFile->header->mesh_count);

  // Get number of triangles
  m_num_triangles = 0;
  m_num_vertices = 0;
  libmd3_mesh *meshp = modelFile->meshes;

  m_boundaries[0] = 0;
  for (int i = 0; i < modelFile->header->mesh_count; ++i, ++meshp) {
    m_num_triangles += meshp->mesh_header->triangle_count;
    m_num_vertices += meshp->mesh_header->vertex_count;
    // Record where the different meshes end
    m_boundaries[i+1] = m_num_triangles;
  }

  // Create data arrays
  m_vertex_data = new short[m_num_vertices * 3];
  m_texel_data  = new float[m_num_vertices * 2];
  m_normal_data = new float[m_num_vertices * 3];
  m_faces       = new unsigned int[m_num_triangles * 3];

  // Get mesh data
  meshp = modelFile->meshes;
  for (int i = 0; i < modelFile->header->mesh_count; ++i, ++meshp) {
    // Get Texture data from Mesh
    if (meshp->mesh_header->skin_count != 0) {
      std::string name = (const char*)(meshp->skins[0].name);
      // Check for texture name overrides in vconf file
      if (m_config.findItem(name, "filename")) {
        name = (std::string)m_config.getItem(name, "filename");
      }
      // Request Texture ID
      m_textures[i] = RenderSystem::getInstance().requestTexture(name);
      m_mask_textures[i] = RenderSystem::getInstance().requestTexture(name, true);
    } else {
      m_textures[i] = 0;
      m_mask_textures[i] = 0;
    }
   
    // Copy data into array.
    memcpy(&m_vertex_data[m_boundaries[i] * 3 * 3], meshp->vertices, meshp->mesh_header->vertex_count * 3 * sizeof(short));

    memcpy(&m_texel_data[m_boundaries[i] * 3 * 2], meshp->texcoords, meshp->mesh_header->vertex_count * 2 * sizeof(float));

    memcpy(&m_normal_data[m_boundaries[i] * 3 * 3], meshp->normals, meshp->mesh_header->vertex_count * 3 * sizeof(float));

    memcpy(&m_faces[m_boundaries[i] * 3], meshp->triangles, meshp->mesh_header->triangle_count * 3 * sizeof(unsigned int));

    // We are using one buffer for all objects, so adjust face vertex 
    // numbers accordingly
    for (int j = 0; j < meshp->mesh_header->triangle_count * 3; ++j) {
      m_faces[m_boundaries[i] *  3 + j] += m_boundaries[i] * 3;
    }
  }

  libmd3_file_free(modelFile);

  m_initialised = true;
  return 0;
}

int LibModelFile::shutdown() {
  assert(m_initialised);
  m_initialised = false;

  //  Clean up OpenGL data
  invalidate();

  // Clear up buffers
  delete [] m_vertex_data;
  m_vertex_data = NULL;

  delete [] m_normal_data;
  m_normal_data = NULL;

  delete [] m_texel_data;
  m_texel_data = NULL;

  delete [] m_faces;
  m_faces = NULL;

  m_num_triangles = 0;

  return 0;
}

void LibModelFile::invalidate() {
  // Clean up Display Lists
  if (glIsList(m_render_list)) glDeleteLists(1, m_render_list);
  if (glIsList(m_select_list)) glDeleteLists(1, m_select_list);

  // Clean up VBO's
  if (glIsBufferARB(m_vbos[0])) glDeleteBuffersARB(1, &m_vbos[0]);
  if (glIsBufferARB(m_vbos[1])) glDeleteBuffersARB(1, &m_vbos[1]);
  if (glIsBufferARB(m_vbos[2])) glDeleteBuffersARB(1, &m_vbos[2]);
  if (glIsBufferARB(m_vbos[3])) glDeleteBuffersARB(1, &m_vbos[3]);
}

void LibModelFile::genVBOs() {
  // Create VBO's if required.
  assert (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT] == true);
  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) {
    // Create VBOS
    glGenBuffersARB(4, &m_vbos[0]);

    // Generate Vertex Array VBO
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbos[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_vertices * 3 * sizeof(short), m_vertex_data, GL_STATIC_DRAW_ARB);
    
    // Generate Normal Array VBO
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbos[1]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_vertices * 3 * sizeof(float), m_normal_data, GL_STATIC_DRAW_ARB);

    // Generate Texel Array VBO
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbos[2]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_num_vertices * 2 * sizeof(float), m_texel_data, GL_STATIC_DRAW_ARB);

    // Generate faces vbo
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vbos[3]);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_num_triangles * 3 * sizeof(unsigned int), m_faces, GL_STATIC_DRAW_ARB);
    
    // Reset buffer status
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }
}
 
void LibModelFile::render(bool select_mode) {
  assert(m_render && "LibModelFile m_render is null");

  // Default material properties
  static float ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  static float shininess = 50.0f;

  // Scale to 1/64 
  glScalef(default_scale, default_scale, default_scale);
 
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  if (sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT]) { // Use Vertex Buffer Objects
    // Generate buffers if they do not exist
    if (!glIsBufferARB(m_vbos[0])) genVBOs();

    // Set material properties
    m_render->setMaterial(&ambient[0], &diffuse[0], &specular[0], shininess, NULL);

    // Bind Vertex Array
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbos[0]);
    glVertexPointer(3, GL_SHORT, 0, NULL);

    // Bind Normal array
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbos[1]);
    glNormalPointer(GL_FLOAT, 0, NULL);

    // Bind texel array
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbos[2]);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    // Bind face data
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_vbos[3]);

    // Render meshes
    for (unsigned int i = 0; i < m_boundaries.size() - 1; ++i) {
      // Switch to required texture 
      if (select_mode) RenderSystem::getInstance().switchTexture(m_mask_textures[i]);
      else RenderSystem::getInstance().switchTexture(m_textures[i]);
      // Calculate how many elements to render
      int count = m_boundaries[i + 1] - m_boundaries[i];
      glDrawElements(GL_TRIANGLES, count * 3,
                     GL_UNSIGNED_INT, (void*)(NULL + (m_boundaries[i] * 3 * sizeof(unsigned int))));
    }

    // Reset VBO buffer
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  } else { // Use Vertex Arrays
    GLuint &list = (select_mode) ? (m_select_list) : (m_render_list);
    // Call display list if created
    if (glIsList(list)) {
      glCallList(list);
    } else { // Generate new display list
      list = glGenLists(1);
      glNewList(list, GL_COMPILE_AND_EXECUTE);

      // Set material properties
      m_render->setMaterial(&ambient[0], &diffuse[0], &specular[0], shininess, NULL);

      // Setup vertex pointers
      glVertexPointer(3, GL_SHORT, 0, m_vertex_data);
      glNormalPointer(GL_FLOAT, 0, m_normal_data);
      glTexCoordPointer(2, GL_FLOAT, 0, m_texel_data);

      // Use the Lock arrays extension if available.
      if (sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY]) glLockArraysEXT(0, m_num_triangles * 3);
      // Start rendering the meshes
      for (unsigned int i = 0; i < m_boundaries.size() - 1; ++i) {
        // Switch to required texture 
        if (select_mode) RenderSystem::getInstance().switchTexture(m_mask_textures[i]);
        else RenderSystem::getInstance().switchTexture(m_textures[i]);
        // Calculate how many elements to render
        int count = m_boundaries[i + 1] - m_boundaries[i];
        glDrawElements(GL_TRIANGLES, count * 3,
                       GL_UNSIGNED_INT, &m_faces[m_boundaries[i] * 3]);
      }
      // We are finished with the vertex arrays now.
      if (sage_ext[GL_EXT_COMPILED_VERTEX_ARRAY]) glUnlockArraysEXT();

      glEndList();
    }
  }
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void LibModelFile::varconf_error_callback(const char *message) {
  fprintf(stderr, "LibModelFile: %s\n", message);
}

} /* namespace Sear */
