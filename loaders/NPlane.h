// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _NPLANE_H_
#define _NPLANE_H_ 1

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
};

} /* namespace Sear */ 
#endif /* _NPLANE_H_ */
