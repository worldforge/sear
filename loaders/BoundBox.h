// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: BoundBox.h,v 1.11 2005-03-15 17:33:58 simon Exp $

#ifndef SEAR_BOUNDBOX_H
#define SEAR_BOUNDBOX_H 1

/*
 * This reprents a model based upon its bounding box.
 *
 */ 

#include <wfmath/axisbox.h>

#include "Model.h"

#include "common/types.h"

namespace Sear {

class BoundBox : public Model {
public:
  /*
   * Constructor stores the bounding box for the basis of this model.
   * wrap indicates how to produce the texture coords. IE 0->1 or 0->size of face
   */ 	
  BoundBox(Render*);

  /*
   * Default destructor
   */ 
  ~BoundBox();
  
  /*
   * Creates the data arrays
   */ 
  int init(WFMath::AxisBox<3>, const std::string &type, bool wrap_texture);

  /*
   * Cleans up object
   */ 
  int shutdown();
  void render(bool); 

  void invalidate();

  bool useTextures() { return m_use_textures; }
  
private:
  static const int m_num_points = 24; // NUmber of points in model

  bool m_initialised;
  bool m_use_textures;
  std::string m_type;

  Vertex_3 m_vertex_data[m_num_points]; // Vertex data
  Texel m_texture_data[m_num_points]; // Texture Co-oridinates data
  Normal m_normal_data[m_num_points]; // Normal data
  unsigned int m_list;
  unsigned int m_list_select;

  int m_texture;
};

} /* namespace Sear */
#endif /* SEAR_BOUNDBOX_H */
