// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: BoundBox.h,v 1.12 2005-06-29 21:19:41 simon Exp $

#ifndef SEAR_BOUNDBOX_H
#define SEAR_BOUNDBOX_H 1

/*
 * This reprents a model based upon its bounding box.
 *
 */ 

#include <wfmath/axisbox.h>

#include "Model.h"

#include "common/types.h"

#include "renderers/RenderSystem.h"

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

private:
  static const int m_num_points = 24; // NUmber of points in model

  bool m_initialised;
  std::string m_texture_name;
  TextureID m_texture_id;

  Vertex_3 m_vertex_data[m_num_points]; // Vertex data
  Texel m_texture_data[m_num_points]; // Texture Co-oridinates data
  Normal m_normal_data[m_num_points]; // Normal data
  unsigned int m_list;
  unsigned int m_list_select;
};

} /* namespace Sear */
#endif /* SEAR_BOUNDBOX_H */
