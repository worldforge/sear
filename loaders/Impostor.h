// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _IMPOSTOR_H_
#define _IMPOSTOR_H_ 1

#include <string>

#include "src/Model.h"
#include "src/Graphics.h"

namespace Sear {
	
class Impostor : public Model {
public:
  Impostor();
  ~Impostor();
  
  bool init(const std::string &, float width, float height);
  void shutdown();
  
  void render(bool); 
  bool useTextures() { return  _use_textures; }

  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_POSITION; }
  
private:
  bool _use_textures;
  static const int _num_points = 8;
  std::string _type;
  float _vertex_data[_num_points][3];
  float _texture_data[_num_points][2];
  float _normal_data[_num_points][3];
};

} /* namespace Sear */ 
#endif /* _IMPOSTOR_H_ */
