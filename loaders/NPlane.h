// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: NPlane.h,v 1.5 2003-04-23 19:41:57 simon Exp $

#ifndef SEAR_NPLANE_H
#define SEAR_NPLANE_H 1

#include <string>

#include "common/types.h"

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
  Vertex_3 *_vertex_data;
  Normal *_normal_data;
  Texel *_texture_data;
  bool _initialised;


  int tex_1, tex_2;
};

} /* namespace Sear */ 
#endif /* SEAR_NPLANE_H */
