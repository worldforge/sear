// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: NPlane.h,v 1.3 2002-09-07 23:27:06 simon Exp $

#ifndef SEAR_NPLANE_H
#define SAER_NPLANE_H 1

#include <string>

#include "src/Model.h"
#include "src/Graphics.h"

namespace Sear {
	
class NPlane : public Model {
public:
  NPlane(Render*);
  ~NPlane();
  
  bool init(const std::string &, unsigned int num_planes, float width, float height);
  void shutdown();
  
  void render(bool); 
  bool useTextures() { return  _use_textures; }

  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_POSITION; }
  
private:
  bool _use_textures;
  unsigned int _num_planes;
  std::string _type;
  float *_vertex_data;
  float *_normal_data;
  float *_texture_data;
  bool _initialised;
};

} /* namespace Sear */ 
#endif /* SEAR_NPLANE_H */
