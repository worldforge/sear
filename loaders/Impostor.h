// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

//$Id: Impostor.h,v 1.6 2002-09-07 23:27:06 simon Exp $

#ifndef SEAR_IMPOSTOR_H
#define SEAR_IMPOSTOR_H 1

#include <string>

#include "src/Model.h"
#include "src/Graphics.h"

namespace Sear {
	
class Impostor : public Model {
public:
  Impostor(Render*);
  ~Impostor();
  
  bool init(const std::string &, float width, float height, bool multi_textures);
  void shutdown();
  
  void render(bool); 
  bool useTextures() { return  _use_textures; }

  Graphics::RotationStyle rotationStyle() { return Graphics::ROS_POSITION; }
  
private:
  bool _use_textures;
  bool _multi_textures;
  static const int _num_points = 8;
  std::string _type;
  float _vertex_data[_num_points][3];
  float _texture_data[_num_points][2];
  float _normal_data[_num_points][3];
  bool _initialised;
};

} /* namespace Sear */ 
#endif /* SEAR_IMPOSTOR_H */
