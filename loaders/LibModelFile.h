// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: LibModelFile.h,v 1.6 2006-01-28 15:35:49 simon Exp $

#ifndef SEAR_LOADERS_LIBMODELFILE_H
#define SEAR_LOADERS_LIBMODELFILE_H 1

/*
 * This reprents a model based upon its bounding box.
 *
 */ 
#include <sigc++/object_slot.h>

#include <varconf/Config.h>

#include <sage/sage.h>
#include <sage/GL.h>

#include "Model.h"

namespace Sear {

class LibModelFile : public Model, public SigC::Object {
public:
  /*
   * Constructor stores the bounding box for the basis of this model.
   * wrap indicates how to produce the texture coords. IE 0->1 or 0->size of face
   */ 	
  LibModelFile(Render*);

  /*
   * Default destructor
   */ 
  ~LibModelFile();
  
  /*
   * Creates the data arrays
   */ 
  int init(const std::string &filename);

  /*
   * Cleans up object
   */ 
  int shutdown();
  void render(bool); 

  void contextCreated();
  void contextDestroyed(bool check);

  void genVBOs(); 

private:
//  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);

  bool m_initialised;

  short *m_vertex_data; // Vertex data
  float *m_normal_data; // Normal data
  float *m_texel_data; // Texture Co-oridinates data
  unsigned int *m_faces;

  int m_num_triangles;
  int m_num_vertices;
  GLuint m_vbos[4];

  GLuint m_render_list;
  GLuint m_select_list;

  std::vector<int> m_boundaries;
  std::vector<int> m_textures;
  std::vector<int> m_mask_textures;

  varconf::Config m_config;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_LIBMODELFILE_H */
