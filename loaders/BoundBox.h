// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: BoundBox.h,v 1.10 2005-01-06 12:46:54 simon Exp $

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
  bool init(WFMath::AxisBox<3>, const std::string &type, bool wrap_texture);

  /*
   * Cleans up object
   */ 
  void shutdown();
  void render(bool); 

  void invalidate();

  bool useTextures() { return _use_textures; }
  
private:
  std::string _type;
  bool _use_textures;
  static const int _num_points = 24; // NUmber of points in model
  Vertex_3 _vertex_data[_num_points]; // Vertex data
  Texel _texture_data[_num_points]; // Texture Co-oridinates data
  Normal _normal_data[_num_points]; // Normal data
  bool _initialised;
  unsigned int _list;
  unsigned int _list_select;

  int texture;
};

} /* namespace Sear */
#endif /* SEAR_BOUNDBOX_H */
