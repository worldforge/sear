// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: LibModelFile.cpp,v 1.18 2006-02-15 09:50:31 simon Exp $

/*
  Debug check list
  * Correct Indices
  * glDrawElements faces pointer data
  * Correct normal calculation

*/

#include <sigc++/object_slot.h>

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

#include "renderers/RenderSystem.h"

#include "LibModelFile.h"

#include "StaticObject.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

static const float default_scale = 1.0f / 64.0f;

LibModelFile::LibModelFile() : Model(), 
  m_initialised(false)
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
  }

  // Get mesh data
  libmd3_mesh *meshp = modelFile->meshes;
  for (int i = 0; i < modelFile->header->mesh_count; ++i, ++meshp) {
    SPtrShutdown<StaticObject> so(new StaticObject());
    so->init();

    // Get Texture data from Mesh
    int texture_id = 0, texture_mask_id = 0;
    if (meshp->mesh_header->skin_count != 0) {
      std::string name = (const char*)(meshp->skins[0].name);
      m_config.clean(name);
      // Check for texture name overrides in vconf file
      if (m_config.findItem(name, "filename")) {
        name = (std::string)m_config.getItem(name, "filename");
      }
      // Request Texture ID
      texture_id = RenderSystem::getInstance().requestTexture(name);
      texture_mask_id = RenderSystem::getInstance().requestTexture(name, true);
    }
    so->setTexture(0, texture_id, texture_mask_id);
    so->setNumPoints(meshp->mesh_header->triangle_count * 3);
    
    so->setAmbient(1.0f,1.0f,1.0f,1.0f);
    so->setDiffuse(1.0f,1.0f,1.0f,1.0f);
    so->setSpecular(1.0f,1.0f,1.0f,1.0f);
    so->setEmission(0.0f,0.0f,0.0f,0.0f);
    so->setShininess(50.0f);

    // Copy data into array.
    so->createVertexData(meshp->mesh_header->vertex_count * 3);
    float *ptr = so->getVertexDataPtr();
    for (int i = 0; i <  meshp->mesh_header->vertex_count * 3; ++i){
      ptr[i] = default_scale * (float)meshp->vertices[i];
    }  
 
    so->copyTextureData(meshp->texcoords, meshp->mesh_header->vertex_count * 2);
    so->copyNormalData(meshp->normals, meshp->mesh_header->vertex_count * 3);

    so->copyIndices(meshp->triangles, meshp->mesh_header->triangle_count * 3);

    m_static_objects.push_back(so);
  }

  libmd3_file_free(modelFile);

  m_initialised = true;
  return 0;
}

int LibModelFile::shutdown() {
  assert(m_initialised);
  m_initialised = false;

  //  Clean up OpenGL data
  contextDestroyed(true);

  return 0;
}

void LibModelFile::contextCreated() {}

void LibModelFile::contextDestroyed(bool check) {
  StaticObjectList::const_iterator I = m_static_objects.begin();
  for (; I != m_static_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->contextDestroyed(check);
  }

}

void LibModelFile::render(bool select_mode) {
  for (StaticObjectList::const_iterator I = m_static_objects.begin(); I != m_static_objects.end(); ++I) {
    SPtrShutdown<StaticObject> so = *I;
    assert(so);
    so->render(select_mode);
  }
}

void LibModelFile::varconf_error_callback(const char *message) {
  fprintf(stderr, "LibModelFile: %s\n", message);
}

} /* namespace Sear */
